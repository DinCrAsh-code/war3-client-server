#include "Game.h"

CGame* g_game = 0;

static const int   kStartInset       = 14;
static const int   kStartGold        = 500;
static const int   kStartLumber      = 150;
static const int   kStartWorkers     = 5;
static const int   kMainMineGold     = 12500;
static const int   kExpansionGold    = 8000;
static const float kDayLengthSeconds = 480.0f;
static const int   kFogInterval      = 10;
static const int   kAiInterval       = 10;
static const int   kSyncInterval     = 8;

bool CGame::Init(u32 seed, int numPlayers)
{
    m_numPlayers = ClampI(numPlayers, 2, kMaxRealPlayers);
    m_rng.Seed(seed);
    m_handles.Init(kMaxHandles);
    m_tick = 0;
    m_turn = 0;
    m_time = 0.0f;
    m_timeOfDay = 8.0f;
    m_gameOver = false;
    m_winner = -1;
    m_lastChecksum = 0;
    memset(m_treeIndex, 0xFF, sizeof(m_treeIndex));

    SetupPlayers(seed);

    Vec2 starts[kMaxRealPlayers];
    for (int i = 0; i < m_numPlayers; ++i)
        starts[i] = m_players[i].m_start;
    m_terrain.Generate(m_rng, starts, m_numPlayers);

    m_pathFinder.Init(&m_terrain);
    m_grid.Init();
    m_fog.Init(&m_terrain);
    m_timers.Init();
    m_replay.Init();
    m_commands.Init(&m_replay);

    for (int i = 0; i < m_terrain.TreeCount(); ++i)
    {
        int cell = m_terrain.TreeCell(i);
        CDestructable* tree = new CDestructable(cell);
        if (!m_handles.Register(tree))
        {
            delete tree;
            break;
        }
        m_treeIndex[cell] = (u16)m_trees.Count();
        m_trees.Add(tree);
    }

    for (int p = 0; p < m_numPlayers; ++p)
        SpawnStartingUnits(p);

    for (int i = 0; i < m_terrain.ExpansionCount(); ++i)
    {
        Vec2 e = m_terrain.Expansion(i);
        int cx = CTerrain::WorldToCell(e.x);
        int cy = CTerrain::WorldToCell(e.y);
        if (!m_terrain.CanBuildFootprint(cx, cy, 3))
            continue;
        CUnit* mine = CreateUnit(UT_GOLDMINE, kPlayerNeutralPassive, FootprintCenter(cx, cy, 3), 0.0f);
        if (mine)
            static_cast<CGoldMine*>(mine)->m_gold = kExpansionGold;
    }

    m_creeps.Init();
    m_creeps.SpawnCamps();

    m_events.Register(EVT_UNIT_DAMAGED, OnUnitDamagedEvent, this);
    for (int p = 0; p < m_numPlayers; ++p)
        m_ai[p].Init(p);

    if (!m_script.Load(g_mapScript, g_mapScriptSize))
        return false;
    m_script.Run();

    UpdateFog();
    return true;
}

void CGame::SetupPlayers(u32 seed)
{
    static const int corners[kMaxRealPlayers][2] =
    {
        { kStartInset, kStartInset },
        { kMapCells - kStartInset, kMapCells - kStartInset },
        { kMapCells - kStartInset, kStartInset },
        { kStartInset, kMapCells - kStartInset },
    };

    for (int i = 0; i < m_numPlayers; ++i)
    {
        Vec2 start = FootprintCenter(corners[i][0], corners[i][1], 4);
        int race = (int)((seed >> (i * 3)) & 1);
        m_players[i].Init(i, race, i, PF_ACTIVE | PF_AI, start);
        m_players[i].m_gold = kStartGold;
        m_players[i].m_lumber = kStartLumber;
    }

    Vec2 center(kMapSize * 0.5f, kMapSize * 0.5f);
    m_players[kPlayerNeutralHostile].Init(kPlayerNeutralHostile, RACE_NEUTRAL, kPlayerNeutralHostile,
                                          PF_ACTIVE | PF_NEUTRAL, center);
    m_players[kPlayerNeutralPassive].Init(kPlayerNeutralPassive, RACE_NEUTRAL, kPlayerNeutralPassive,
                                          PF_ACTIVE | PF_NEUTRAL | PF_PASSIVE, center);
}

void CGame::SpawnStartingUnits(int player)
{
    CPlayer& p = m_players[player];
    Vec2 start = p.m_start;
    Vec2 center(kMapSize * 0.5f, kMapSize * 0.5f);
    Vec2 toCenter = Normalize(center - start);
    Vec2 side(-toCenter.y, toCenter.x);

    CreateUnit(TownHallTypeForRace(p.m_race), player, start, AngleTo(start, center));

    for (int attempt = 0; attempt < 2; ++attempt)
    {
        Vec2 spot = start + side * (8.0f * kCellSize * (attempt ? -1.0f : 1.0f)) - toCenter * (2.0f * kCellSize);
        int cx = CTerrain::WorldToCell(spot.x);
        int cy = CTerrain::WorldToCell(spot.y);
        if (!m_terrain.CanBuildFootprint(cx, cy, 3))
            continue;
        CUnit* mine = CreateUnit(UT_GOLDMINE, kPlayerNeutralPassive, FootprintCenter(cx, cy, 3), 0.0f);
        if (mine)
            static_cast<CGoldMine*>(mine)->m_gold = kMainMineGold;
        break;
    }

    int worker = WorkerTypeForRace(p.m_race);
    for (int i = 0; i < kStartWorkers; ++i)
    {
        Vec2 spot = start + toCenter * (3.5f * kCellSize) + side * ((float)(i - kStartWorkers / 2) * kCellSize);
        int cx = CTerrain::WorldToCell(spot.x);
        int cy = CTerrain::WorldToCell(spot.y);
        if (!m_terrain.FindNearestWalkable(cx, cy, 6))
            continue;
        CreateUnit(worker, player, Vec2(CTerrain::CellCenter(cx), CTerrain::CellCenter(cy)), AngleTo(start, center));
    }
}

CUnit* CGame::CreateUnit(int type, int owner, const Vec2& pos, float facing, bool constructing)
{
    if (type < 0 || type >= UT_COUNT || owner < 0 || owner >= kMaxPlayers)
        return 0;

    const UnitTypeData* d = GetUnitType(type);
    CUnit* u;
    if (d->flags & UTF_GOLDMINE)
        u = new CGoldMine(owner, kMainMineGold);
    else if (d->flags & UTF_HERO)
        u = new CHero(type, owner);
    else if (d->flags & UTF_STRUCTURE)
        u = new CBuilding(type, owner);
    else
        u = new CUnit(type, owner);

    if (!m_handles.Register(u))
    {
        delete u;
        return 0;
    }

    u->m_pos = pos;
    u->m_facing = facing;
    u->m_guardPos = pos;
    u->m_orderOrigin = pos;
    if (d->flags & UTF_STRUCTURE)
    {
        m_terrain.MarkFootprint(CTerrain::WorldToCell(pos.x), CTerrain::WorldToCell(pos.y), d->footprint, true);
        if (constructing)
            u->m_state |= US_CONSTRUCTING;
    }

    u->RecalcStats();
    u->m_life = u->m_maxLife;
    u->m_mana = (d->flags & UTF_HERO) ? u->m_stats.maxMana : u->m_stats.maxMana * 0.5f;

    m_units.Add(u);
    m_grid.Insert(u);
    m_players[owner].OnUnitCreated(u, !constructing);
    return u;
}

CItem* CGame::CreateItem(int type, const Vec2& pos)
{
    CItem* item = new CItem(type);
    if (!m_handles.Register(item))
    {
        delete item;
        return 0;
    }
    item->m_pos = pos;
    m_items.Add(item);
    return item;
}

void CGame::AddMissile(CMissile* missile)
{
    m_missiles.Add(missile);
}

void CGame::OnUnitDeath(CUnit* unit, CUnit* killer)
{
    CPlayer& owner = m_players[unit->m_owner];
    owner.OnUnitRemoved(unit);
    ++owner.m_unitsLost;

    if (unit->IsStructure())
    {
        m_terrain.MarkFootprint(CTerrain::WorldToCell(unit->m_pos.x), CTerrain::WorldToCell(unit->m_pos.y),
                                unit->m_data->footprint, false);
    }

    AwardKill(killer, unit);
    if (unit->m_owner == kPlayerNeutralHostile)
        m_creeps.OnCreepDeath(unit, killer);
    m_events.Fire(EVT_UNIT_DEATH, unit, killer, 0.0f, unit->m_type);
    m_script.FireDeathTriggers(unit, killer);
}

void CGame::OnUnitDamagedEvent(const EventData& e, void* context)
{
    CGame* game = static_cast<CGame*>(context);
    if (e.unit && e.unit->m_owner == kPlayerNeutralHostile)
        game->m_creeps.OnCreepDamaged(e.unit, e.other);
}

// ---------------------------------------------------------------------------
// Lookups

CWidget* CGame::GetWidget(Handle h) const
{
    if (!h)
        return 0;
    return static_cast<CWidget*>(m_handles.Lookup(h));
}

CUnit* CGame::GetUnit(Handle h) const
{
    CWidget* w = GetWidget(h);
    return (w && w->m_kind == WK_UNIT) ? static_cast<CUnit*>(w) : 0;
}

CItem* CGame::GetItem(Handle h) const
{
    CWidget* w = GetWidget(h);
    return (w && w->m_kind == WK_ITEM) ? static_cast<CItem*>(w) : 0;
}

CDestructable* CGame::GetTree(Handle h) const
{
    CWidget* w = GetWidget(h);
    return (w && w->m_kind == WK_DESTRUCTABLE) ? static_cast<CDestructable*>(w) : 0;
}

CDestructable* CGame::TreeAtCell(int cell) const
{
    if (cell < 0 || cell >= kCellCount || m_treeIndex[cell] == 0xFFFF)
        return 0;
    return m_trees[m_treeIndex[cell]];
}

CDestructable* CGame::FindNearestTree(const Vec2& p, float maxRange) const
{
    static const int dx[4] = { 1, -1, 0, 0 };
    static const int dy[4] = { 0, 0, 1, -1 };

    CDestructable* best = 0;
    float bestD = maxRange * maxRange;
    for (int i = 0; i < m_trees.Count(); ++i)
    {
        CDestructable* t = m_trees[i];
        if (!t->IsAlive())
            continue;
        float d = DistSq(t->m_pos, p);
        if (d >= bestD)
            continue;

        int cx = t->m_cell % kMapCells;
        int cy = t->m_cell / kMapCells;
        bool reachable = false;
        for (int k = 0; k < 4 && !reachable; ++k)
            reachable = m_terrain.IsWalkable(cx + dx[k], cy + dy[k]);
        if (!reachable)
            continue;

        best = t;
        bestD = d;
    }
    return best;
}

CUnit* CGame::FindDropoff(int player, const Vec2& p, bool lumber) const
{
    CUnit* best = 0;
    float bestD = 1e12f;
    for (int i = 0; i < m_units.Count(); ++i)
    {
        CUnit* u = m_units[i];
        if (u->m_owner != player || !u->IsAlive() || (u->m_state & US_CONSTRUCTING))
            continue;
        u16 need = lumber ? (u16)(UTF_TOWNHALL | UTF_LUMBERDROP) : (u16)UTF_TOWNHALL;
        if (!(u->m_data->flags & need))
            continue;
        float d = DistSq(u->m_pos, p);
        if (d < bestD)
        {
            bestD = d;
            best = u;
        }
    }
    return best;
}

CGoldMine* CGame::FindNearestMine(const Vec2& p, float maxRange) const
{
    CGoldMine* best = 0;
    float bestD = maxRange * maxRange;
    for (int i = 0; i < m_units.Count(); ++i)
    {
        CUnit* u = m_units[i];
        if (!(u->m_data->flags & UTF_GOLDMINE) || !u->IsAlive())
            continue;
        CGoldMine* mine = static_cast<CGoldMine*>(u);
        if (mine->m_gold <= 0)
            continue;
        float d = DistSq(u->m_pos, p);
        if (d < bestD)
        {
            bestD = d;
            best = mine;
        }
    }
    return best;
}

CItem* CGame::FindNearestItem(const Vec2& p, float maxRange) const
{
    CItem* best = 0;
    float bestD = maxRange * maxRange;
    for (int i = 0; i < m_items.Count(); ++i)
    {
        CItem* item = m_items[i];
        if (!item->IsAlive() || item->m_carrier || (item->m_state & US_HIDDEN))
            continue;
        float d = DistSq(item->m_pos, p);
        if (d < bestD)
        {
            bestD = d;
            best = item;
        }
    }
    return best;
}

bool CGame::IsNight() const
{
    return m_timeOfDay < 6.0f || m_timeOfDay >= 18.0f;
}

bool CGame::IsVisibleTo(int player, const CUnit* unit) const
{
    if (unit->m_state & US_INVISIBLE)
        return m_players[player].IsAlly(unit->m_owner);
    if (player >= kMaxRealPlayers || m_players[player].IsAlly(unit->m_owner))
        return true;
    return m_fog.IsVisible(player, unit->m_pos);
}

// ---------------------------------------------------------------------------
// Simulation

void CGame::UpdateTimeOfDay()
{
    bool wasNight = IsNight();
    m_timeOfDay += kTickSeconds * 24.0f / kDayLengthSeconds;
    if (m_timeOfDay >= 24.0f)
        m_timeOfDay -= 24.0f;

    if (wasNight != IsNight())
    {
        for (int i = 0; i < m_units.Count(); ++i)
            m_units[i]->m_state |= US_STATS_DIRTY;
    }
}

void CGame::UpdateAI()
{
    for (int p = 0; p < m_numPlayers; ++p)
    {
        if ((m_tick + p * 3) % kAiInterval == 0)
            m_ai[p].Think();
    }
}

void CGame::UpdateUnits()
{
    for (int i = 0; i < m_units.Count(); ++i)
        m_units[i]->Update(kTickSeconds);
}

void CGame::UpdateMissiles()
{
    for (int i = 0; i < m_missiles.Count(); ++i)
        m_missiles[i]->Update(kTickSeconds);

    int live = 0;
    for (int i = 0; i < m_missiles.Count(); ++i)
    {
        CMissile* m = m_missiles[i];
        if (m->m_done)
            delete m;
        else
            m_missiles[live++] = m;
    }
    m_missiles.SetCount(live);
}

void CGame::UpdateFog()
{
    for (int p = 0; p < m_numPlayers; ++p)
    {
        m_fog.BeginUpdate(p);
        const CPlayer& player = m_players[p];
        for (int i = 0; i < m_units.Count(); ++i)
        {
            CUnit* u = m_units[i];
            if (!u->IsAlive() || (u->m_state & US_HIDDEN) || !player.IsAlly(u->m_owner))
                continue;
            m_fog.Reveal(p, u->m_pos, u->SightRadius(), u->IsFlying());
        }
    }
}

void CGame::DestroyUnit(CUnit* unit)
{
    m_grid.Remove(unit);
    m_handles.Unregister(unit->m_handle);
    int index = m_units.Find(unit);
    if (index >= 0)
        m_units.RemoveSwap(index);
    delete unit;
}

void CGame::CleanupDead()
{
    for (int i = m_units.Count() - 1; i >= 0; --i)
    {
        if (m_units[i]->m_state & US_REMOVE)
            DestroyUnit(m_units[i]);
    }

    for (int i = m_items.Count() - 1; i >= 0; --i)
    {
        CItem* item = m_items[i];
        if (!(item->m_state & US_REMOVE))
            continue;
        m_handles.Unregister(item->m_handle);
        m_items.RemoveSwap(i);
        delete item;
    }
}

u32 CGame::ComputeChecksum() const
{
    u32 h = HashCombine(0x9747B28Cu, (u32)m_tick);
    h = HashCombine(h, m_rng.State());
    for (int p = 0; p < kMaxPlayers; ++p)
    {
        h = HashCombine(h, (u32)m_players[p].m_gold);
        h = HashCombine(h, (u32)m_players[p].m_lumber);
        h = HashCombine(h, (u32)m_players[p].m_foodUsed);
    }
    for (int i = 0; i < m_units.Count(); ++i)
    {
        const CUnit* u = m_units[i];
        h = HashCombine(h, u->m_handle);
        h = HashCombine(h, ((u32)u->m_type << 8) | u->m_order.id);
        h = HashCombine(h, (u32)(s32)u->m_pos.x ^ ((u32)(s32)u->m_pos.y << 16));
        h = HashCombine(h, (u32)(s32)u->m_life);
    }
    return h;
}

void CGame::SetDefeated(int player)
{
    CPlayer& p = m_players[player];
    if (p.IsDefeated())
        return;
    p.m_flags |= PF_DEFEATED;
    for (int i = 0; i < m_units.Count(); ++i)
    {
        CUnit* u = m_units[i];
        if (u->m_owner == player && u->IsAlive())
            u->Kill(0);
    }
}

void CGame::EndGame(int winner)
{
    m_gameOver = true;
    m_winner = winner;
}

// Time limit reached: the best-scoring surviving player wins.
void CGame::CheckVictory()
{
    int best = -1;
    int bestScore = -1;
    for (int p = 0; p < m_numPlayers; ++p)
    {
        const CPlayer& pl = m_players[p];
        if (pl.IsDefeated())
            continue;
        int score = pl.m_goldMined + pl.m_lumberHarvested + pl.m_unitsKilled * 50 + pl.StructureCount() * 100;
        if (score > bestScore)
        {
            bestScore = score;
            best = p;
        }
    }
    EndGame(best);
}

void CGame::Tick()
{
    m_time = (float)m_tick * kTickSeconds;

    if (m_tick % kTurnTicks == 0)
        m_commands.ExecuteTurn(m_turn);

    m_timers.Advance(m_time);
    UpdateTimeOfDay();
    UpdateAI();
    UpdateUnits();
    UpdateMissiles();
    m_creeps.Update(kTickSeconds);
    if (m_tick % kFogInterval == 0)
        UpdateFog();
    CleanupDead();

    if (m_tick % kTurnTicks == kTurnTicks - 1)
    {
        m_commands.EndTurn(m_turn);
        ++m_turn;
        if (m_turn % kSyncInterval == 0)
        {
            m_lastChecksum = ComputeChecksum();
            m_replay.RecordSync(m_turn, m_lastChecksum);
        }
    }
    ++m_tick;
}

u32 CGame::Run(int maxTicks)
{
    while (!m_gameOver && m_tick < maxTicks)
        Tick();
    if (!m_gameOver)
        CheckVictory();

    u32 replayCrc = m_replay.Finalize();
    bool verified = m_replay.Verify();

    u32 result = HashCombine(ComputeChecksum(), replayCrc);
    result = HashCombine(result, (u32)(m_winner + 1) | (verified ? 0x100u : 0u));
    result = HashCombine(result, (u32)m_script.OpsExecuted());
    return result;
}

void CGame::Shutdown()
{
    for (int i = 0; i < m_missiles.Count(); ++i)
        delete m_missiles[i];
    m_missiles.Clear();

    for (int i = 0; i < m_units.Count(); ++i)
    {
        m_handles.Unregister(m_units[i]->m_handle);
        delete m_units[i];
    }
    m_units.Clear();

    for (int i = 0; i < m_items.Count(); ++i)
        delete m_items[i];
    m_items.Clear();

    for (int i = 0; i < m_trees.Count(); ++i)
        delete m_trees[i];
    m_trees.Clear();

    m_handles.Shutdown();
}
