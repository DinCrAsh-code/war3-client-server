#include "Game.h"

// Lockstep command pipeline. Commands issued during turn T are bit-packed
// into that turn's outgoing packet; at the end of the turn the packet is
// scheduled for execution kTurnLatency turns later and recorded in the
// replay. Every command in a packet is prefixed by a 1 bit, and a 0 bit
// terminates the packet.

static const float kFormationSpacing = 72.0f;

void CCommandSystem::Init(CReplay* replay)
{
    m_replay = replay;
    for (int i = 0; i < (int)COUNTOF(m_scheduled); ++i)
    {
        m_scheduled[i].turn = -1;
        m_scheduled[i].length = 0;
    }
    m_writer.Init(m_outgoing, sizeof(m_outgoing));
    m_pendingCommands = 0;
    m_executed = 0;
    m_rejected = 0;
}

bool CCommandSystem::Encode(CBitWriter& w, const Command& cmd)
{
    w.WriteBits(cmd.type, 2);
    w.WriteBits(cmd.player, 3);
    w.WriteBits(cmd.numUnits, 4);
    w.WriteBits(cmd.queued ? 1u : 0u, 1);

    // First handle in full, the rest as signed deltas (unit groups tend to
    // have neighbouring handles).
    for (int i = 0; i < cmd.numUnits; ++i)
    {
        if (i == 0)
            w.WriteVar(cmd.units[0]);
        else
            w.WriteSigned((s32)(cmd.units[i] - cmd.units[i - 1]));
    }

    const COrder& o = cmd.order;
    w.WriteBits(o.id, 5);
    w.WriteBits(o.flags, 4);
    w.WriteVar(o.param);
    if (o.flags & (OF_TARGET_UNIT | OF_TARGET_ITEM))
        w.WriteVar(o.target);
    if (o.flags & OF_TARGET_POINT)
    {
        w.WriteQuant(o.point.x, 0.0f, kMapSize, 16);
        w.WriteQuant(o.point.y, 0.0f, kMapSize, 16);
    }
    return !w.Overflowed();
}

bool CCommandSystem::Decode(CBitReader& r, Command& cmd)
{
    cmd.type = (u8)r.ReadBits(2);
    cmd.player = (u8)r.ReadBits(3);
    cmd.numUnits = (u8)r.ReadBits(4);
    cmd.queued = (u8)r.ReadBits(1);
    if (cmd.numUnits > kMaxCommandUnits || cmd.type >= CMD_COUNT)
        return false;

    for (int i = 0; i < cmd.numUnits; ++i)
    {
        if (i == 0)
            cmd.units[0] = r.ReadVar();
        else
            cmd.units[i] = (Handle)((s32)cmd.units[i - 1] + r.ReadSigned());
    }

    COrder& o = cmd.order;
    o.id = (u8)r.ReadBits(5);
    o.flags = (u8)r.ReadBits(4);
    o.param = (u16)r.ReadVar();
    o.target = 0;
    o.point = Vec2(0.0f, 0.0f);
    if (o.flags & (OF_TARGET_UNIT | OF_TARGET_ITEM))
        o.target = r.ReadVar();
    if (o.flags & OF_TARGET_POINT)
    {
        o.point.x = r.ReadQuant(0.0f, kMapSize, 16);
        o.point.y = r.ReadQuant(0.0f, kMapSize, 16);
    }
    return !r.Failed();
}

bool CCommandSystem::Queue(const Command& cmd)
{
    // Worst-case command is well under 128 bytes; refuse rather than
    // leave a truncated command in the packet.
    if (m_writer.BitPosition() > (sizeof(m_outgoing) - 128) * 8)
    {
        ++m_rejected;
        return false;
    }
    m_writer.WriteBits(1, 1);
    Encode(m_writer, cmd);
    ++m_pendingCommands;
    return true;
}

void CCommandSystem::EndTurn(int turn)
{
    m_writer.WriteBits(0, 1);
    u32 length = m_writer.Finish();

    int executeTurn = turn + kTurnLatency;
    TurnPacket& packet = m_scheduled[executeTurn % (int)COUNTOF(m_scheduled)];
    packet.turn = executeTurn;
    packet.length = length;
    memcpy(packet.data, m_outgoing, length);

    if (m_pendingCommands > 0)
        m_replay->RecordTurn(executeTurn, packet.data, length);

    m_writer.Init(m_outgoing, sizeof(m_outgoing));
    m_pendingCommands = 0;
}

void CCommandSystem::ExecuteTurn(int turn)
{
    TurnPacket& packet = m_scheduled[turn % (int)COUNTOF(m_scheduled)];
    if (packet.turn != turn)
        return;
    packet.turn = -1;

    CBitReader reader;
    reader.Init(packet.data, packet.length);
    while (!reader.Failed() && reader.ReadBits(1) == 1)
    {
        Command cmd;
        if (!Decode(reader, cmd))
        {
            ++m_rejected;
            break;
        }
        Apply(cmd);
    }
}

// Group moves spread units into a square formation around the target.
static Vec2 FormationSlot(const Vec2& center, int index, int count)
{
    int cols = 1;
    while (cols * cols < count)
        ++cols;
    int row = index / cols;
    int col = index % cols;
    float ox = ((float)col - (float)(cols - 1) * 0.5f) * kFormationSpacing;
    float oy = ((float)row - (float)((count - 1) / cols) * 0.5f) * kFormationSpacing;
    return Vec2(center.x + ox, center.y + oy);
}

void CCommandSystem::Apply(const Command& cmd)
{
    if (cmd.player >= kMaxPlayers || g_game->m_players[cmd.player].IsDefeated())
    {
        ++m_rejected;
        return;
    }

    for (int i = 0; i < cmd.numUnits; ++i)
    {
        CUnit* u = g_game->GetUnit(cmd.units[i]);
        if (!u || u->m_owner != cmd.player || !u->IsAlive())
        {
            ++m_rejected;
            continue;
        }

        switch (cmd.type)
        {
        case CMD_ORDER:
            {
                COrder o = cmd.order;
                bool spread = cmd.numUnits > 1 && (o.flags & OF_TARGET_POINT) &&
                              (o.id == ORD_MOVE || o.id == ORD_ATTACK_MOVE);
                if (spread)
                    o.point = FormationSlot(cmd.order.point, i, cmd.numUnits);
                u->IssueOrder(o, cmd.queued != 0);
            }
            break;

        case CMD_LEARN:
            if (u->IsHero())
                static_cast<CHero*>(u)->LearnAbility(cmd.order.param);
            break;

        case CMD_AUTOCAST:
            {
                CAbility* a = u->FindAbility(cmd.order.param);
                if (a && (a->Data()->flags & ABF_AUTOCAST))
                    a->m_autoCast = cmd.order.target != 0;
            }
            break;

        default:
            break;
        }
        ++m_executed;
    }
}
