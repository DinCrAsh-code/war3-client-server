//============================================================================
//  0x6F403890 / 0x6F403770 / 0x6F403810 - CPlayerTallyList::
//  TallyCounterAdjustA/B/C.  Player/playerscore.h's own note on the trio:
//  each finds or creates the tally entry for `typeId` (a call into
//  CDependencyAliasTableOwner::FindOrCreateRecord, 0x6F4036B0 -
//  Item/ability_typedep.h/.cpp, already reconstructed for real; the
//  `TSHashTable` closure underneath it is a module of its own and stays
//  out of scope here exactly as it does there), then bumps one or two of
//  that entry's own counters - a different pair for each of the three -
//  and conditionally republishes the change onto the caller's own object
//  through the flag byte the found entry carries (its own +0x30, already
//  named `m_flags` by ability_typedep.h).
//
//  `this` (ecx) is handed straight to FindOrCreateRecord with no
//  adjustment, so CPlayerTallyList and CDependencyAliasTableOwner share
//  one `this` pointer here - the record cache is CPlayerTallyList's own
//  embedded dependency-alias table, reused for the unit-creation tally.
//
//  B (0x6F403770) was this repo's first of the three; A and C
//  (createunit-H3 follow-up, PART 2) reuse the exact same shape - same
//  "near-identical siblings, one TU" pattern playerscore_tallyapply.cpp's
//  own header documents for TallyOnUnitCreated1/2/3 - consecutive
//  addresses (0xA0 bytes apart), same receiver, same call into the same
//  shared helper, and none of the three calls another.
//============================================================================
#include "playerscore.h"
#include "ability_typedep.h"

//  CPlayerTallyList's own two counters each of A/B touches, beyond
//  CDependencyAliasTableOwner's own small header (+0x00/+0x04) - not
//  otherwise identified, and not documented in playerscore.h itself since
//  A's own +0x54/+0x5C pair is out of this agent's scope to confirm.
//  Raw offset casts from `this` rather than a derived struct: deriving
//  from CPlayerTallyList and appending a reserved gap places the new
//  members past CPlayerTallyList's own (larger than 0x58) real size, not
//  at the absolute offsets the shipped code actually touches.

//----------------------------------------------------------------------------
//  0x6F403770 - CPlayerTallyList::TallyCounterAdjustB.
//----------------------------------------------------------------------------
void CPlayerTallyList::TallyCounterAdjustB(unsigned int typeId, int flag)
{
    CDependencyAliasRecord* record =
        ((CDependencyAliasTableOwner*)this)->FindOrCreateRecord(typeId, 1);

    record->m_field28 += 1;

    if (flag == 0)
        record->m_field1C += 1;

    unsigned int classBits = record->m_flags;
    if (classBits & 4)
    {
        *(unsigned int*)((char*)this + 0x58) += 1;
        return;
    }
    if (classBits & 8)
    {
        *(unsigned int*)((char*)this + 0x60) += 1;
    }
}

//----------------------------------------------------------------------------
//  0x6F403890 - CPlayerTallyList::TallyCounterAdjustA.
//
//  createunit-H3 follow-up, PART 2: B's own real-body shape transfers
//  directly - same FindOrCreateRecord call, same m_flags-driven tail - only
//  which fields get touched, and by how much, differs per variant.  A picks
//  one of two counters to bump by `flag` (m_field24 when set, m_field20
//  when clear - already-named fields, ability_typedep.h's own +0x20/+0x24
//  "kind bit 1/2" pair), then bumps one of this list's own +0x54/+0x5C
//  counters the same way B bumps +0x58/+0x60.
//----------------------------------------------------------------------------
void CPlayerTallyList::TallyCounterAdjustA(unsigned int typeId, int flag)
{
    CDependencyAliasRecord* record =
        ((CDependencyAliasTableOwner*)this)->FindOrCreateRecord(typeId, 1);

    if (flag != 0)
        record->m_field24 += 1;
    else
        record->m_field20 += 1;

    unsigned int classBits = record->m_flags;
    if (classBits & 4)
    {
        *(unsigned int*)((char*)this + 0x54) += 1;
        return;
    }
    if (classBits & 8)
    {
        *(unsigned int*)((char*)this + 0x5C) += 1;
    }
}

//----------------------------------------------------------------------------
//  0x6F403810 - CPlayerTallyList::TallyCounterAdjustC.
//
//  The busiest of the three: always bumps m_field28 (like B) *and* always
//  bumps m_field1C (unconditionally here, not gated on `flag` the way B
//  gates its own +0x1C bump), decrements whichever of m_field20/m_field24
//  `flag` picks (the same picking rule as A, just subtracting instead of
//  adding), and on the tail end touches *both* of this list's own counters
//  for the matched class - decrementing the first (+0x54/+0x5C) and
//  incrementing the second (+0x58/+0x60) - rather than just the one A and B
//  each touch alone.
//----------------------------------------------------------------------------
void CPlayerTallyList::TallyCounterAdjustC(unsigned int typeId, int flag)
{
    CDependencyAliasRecord* record =
        ((CDependencyAliasTableOwner*)this)->FindOrCreateRecord(typeId, 1);

    record->m_field28 += 1;

    //  Not a shared tail: `flag != 0` skips the m_field1C bump entirely
    //  (a real `jmp` straight to the m_flags read), it does not merely
    //  reorder it - the shipped code's own short jump covers both the
    //  m_field20 decrement *and* the m_field1C increment below it.
    if (flag != 0)
    {
        record->m_field24 -= 1;
    }
    else
    {
        record->m_field20 -= 1;
        record->m_field1C += 1;
    }

    unsigned int classBits = record->m_flags;
    if (classBits & 4)
    {
        *(unsigned int*)((char*)this + 0x54) -= 1;
        *(unsigned int*)((char*)this + 0x58) += 1;
        return;
    }
    if (classBits & 8)
    {
        *(unsigned int*)((char*)this + 0x5C) -= 1;
        *(unsigned int*)((char*)this + 0x60) += 1;
    }
}
