//============================================================================
//  0x6F40B250 - CUnit's own dependency-alias lookup: forwards to
//  CDependencyAliasTableOwner::FindOrCreateRecord (already reconstructed,
//  Item/ability_typedep.h) through the table-owner pointer parked at
//  CUnit's own +0x2D0 (Unit/unit.h declares that offset `m_int2D0`, an
//  unsigned int, from an earlier already-matched target; this function's
//  own read shows it is really used as a pointer, but re-typing another
//  target's already-scored field is out of this session's own scope, so
//  the read below goes through a raw offset instead of that struct member)
//  with `allowCreate = 1`, then bumps the found record's own new
//  `m_refCount2C` field (ability_typedep.h) by one before returning it.
//
//  The two addresses are one combined chunk in agent_worktrees (IDA folded
//  a shared tail into this entry), so both are written here as the one
//  reconstruction.
//============================================================================
#include "ability_typedep.h"

//  edx is never read - typeId arrives on the stack (the outer 0x6F40B250
//  entry point adds no frame of its own before jumping here, so its own
//  one stack argument is this function's), and the dummy edx parameter is
//  what keeps __fastcall from putting it there instead
//  (docs/msvc-vc8-idioms.md, "A receiver in ecx with a second argument in
//  edx is a free __fastcall").
CDependencyAliasRecord* __fastcall CUnitFindOrCreateDependencyAlias(
        void* unit, void* /*unused edx*/, unsigned int typeId)
{
    CDependencyAliasTableOwner* table =
        *(CDependencyAliasTableOwner**)((char*)unit + 0x2D0);

    CDependencyAliasRecord* record = table->FindOrCreateRecord(typeId, 1);
    if (record)
        record->m_refCount2C++;

    return record;
}
