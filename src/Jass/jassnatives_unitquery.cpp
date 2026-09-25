//============================================================================
//  The JASS `unit` query natives that answer a question about a unit and a
//  *player*: who owns it, who can see it, who is allied to it.
//
//  Six entry points from the registration table at 0x6F3D4020, all
//  `(Hunit;Hplayer;)B` and all __cdecl:
//
//      IsUnitOwnedByPlayer  sub_6F3C7A30
//      IsUnitAlly           sub_6F3C7A70
//      IsUnitEnemy          sub_6F3C7AD0
//      IsUnitVisible        sub_6F3C7AF0
//      IsUnitDetected       sub_6F3C7C80
//      IsUnitInvisible      sub_6F3C7CC0
//
//  They are one translation unit because IsUnitEnemy *calls* IsUnitAlly -
//  a real __cdecl call with both handles pushed again, not a shared helper
//  - so the two may not be folded into each other, and because the other
//  four are the same eight-instruction preamble with a different question
//  on the end.
//
//  **The shared refusal tail.**  Every one of these resolves two handles
//  and answers 0 if either fails, and the shipped code reaches one
//  `xor eax,eax` / `pop` / `retn` block from both tests - the second by
//  jumping *backwards* to it.  That layout is what two separate
//  `return 0;` statements produce; a single `if (!unit || !player)` sinks
//  one tail to the end of the body instead.  Same split
//  jassnatives_life.cpp's GetUnitState records, and the opposite of the
//  nested-guard shape jassunititeminslot_native.cpp wants.
//
//  A JASS `player` handle resolves to SJassPlayer, whose +0x30 is the
//  player *index* - a byte, which is why every one of these widens it with
//  `movzx` before use.
//============================================================================
#include "jassnatives.h"
#include "unit.h"
#include "jassrelationagent.h"

//----------------------------------------------------------------------------
//  0x6F3C7A30 - IsUnitOwnedByPlayer.
//
//  The index is read into a named local *before* the virtual call, which is
//  the shipped order (`movzx edi,...` at 0x6F3C7A52, the `call edx` at
//  0x6F3C7A60).  Comparing `player->m_playerIndex` in place would sink the
//  load past the call, because MSVC evaluates the operand it cannot clobber
//  last.
//----------------------------------------------------------------------------
int __cdecl JASS_IsUnitOwnedByPlayer(int hUnit, int hPlayer)
{
    CUnit* unit = ResolveUnitHandle(hUnit);
    if (!unit)
        return 0;
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);
    if (!player)
        return 0;

    unsigned int index = player->m_playerIndex;
    return index == (unsigned int)unit->GetOwningPlayerIndex();
}

//----------------------------------------------------------------------------
//  0x6F3C7AF0 - IsUnitVisible.  Slot 63 (+0xFC) with (index, 0, 4), and its
//  result returned as it stands - the shipped tail is `call eax` / `pop esi`
//  / `retn`, with no widening of any kind.
//----------------------------------------------------------------------------
int __cdecl JASS_IsUnitVisible(int hUnit, int hPlayer)
{
    CUnit* unit = ResolveUnitHandle(hUnit);
    if (!unit)
        return 0;
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);
    if (!player)
        return 0;

    return unit->SubmitToPlayerTable(player->m_playerIndex, 0, 4);
}

//----------------------------------------------------------------------------
//  0x6F3C7CC0 - IsUnitInvisible.  The same query, negated.
//
//  `neg`/`sbb`/`add 1` is MSVC's branchless `!x` for a non-bool int, so the
//  source is a plain `!`, not `== 0` (which emits `test`/`sete`).
//----------------------------------------------------------------------------
int __cdecl JASS_IsUnitInvisible(int hUnit, int hPlayer)
{
    CUnit* unit = ResolveUnitHandle(hUnit);
    if (!unit)
        return 0;
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);
    if (!player)
        return 0;

    return !unit->SubmitToPlayerTable(player->m_playerIndex, 0, 4);
}

//----------------------------------------------------------------------------
//  0x6F3C7C80 - IsUnitDetected.  A different query on the same pair: the
//  detection masks at CUnit+0x130, asked for both kinds at once (-1).
//----------------------------------------------------------------------------
int __cdecl JASS_IsUnitDetected(int hUnit, int hPlayer)
{
    CUnit* unit = ResolveUnitHandle(hUnit);
    if (!unit)
        return 0;
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);
    if (!player)
        return 0;

    return unit->QueryDetection(player->m_playerIndex, -1);
}

//----------------------------------------------------------------------------
//  0x6F3C7A70 - IsUnitAlly.
//
//  Not a flag on the unit: the unit's owning player index picks a *bit*,
//  and the bit is looked up in the other player's own relation agent.
//  `1 << owner` against the mask that agent's +0x38 field publishes.
//
//  `and`/`neg`/`sbb`/`neg` is MSVC's `!!(x & bit)` - the double negation
//  that turns any non-zero into exactly 1 - so the source has to produce an
//  int from the masked test, not return the mask.
//----------------------------------------------------------------------------
int __cdecl JASS_IsUnitAlly(int hUnit, int hPlayer)
{
    CUnit* unit = ResolveUnitHandle(hUnit);
    if (!unit)
        return 0;
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);
    if (!player)
        return 0;

    int owner = unit->GetOwningPlayerIndex();

    //  The agent in a named local and the field read off it, not chained:
    //  chained, MSVC forms the field address with `mov`/`add` where the
    //  shipped code has one `lea` - the same effect unit_playermasksearch.cpp
    //  records for this identical pair of calls.
    SJassRelationAgent* agent =
        ((SJassPlayerRelations*)player)->EnsureRelationAgent();
    unsigned int mask = (unsigned int)agent->m_player.QueryField78();

    return !!(mask & (1u << owner));
}

//----------------------------------------------------------------------------
//  0x6F3C7AD0 - IsUnitEnemy.  Literally "not an ally": the shipped body
//  pushes both handles again and __cdecl-calls IsUnitAlly, then negates.
//  Ten instructions, no resolve of its own - so the two handles are never
//  validated here, and an invalid one answers 1 (not-an-ally) rather than
//  0.  That asymmetry is the shipped behaviour, not a defect in this
//  reconstruction.
//----------------------------------------------------------------------------
int __cdecl JASS_IsUnitEnemy(int hUnit, int hPlayer)
{
    return !JASS_IsUnitAlly(hUnit, hPlayer);
}
