//============================================================================
//  0x6F079F40 - SUnitAbilityHost::RemoveAllAbilities.  Same +0x1DC ability
//  chain unit_flagbit8.cpp's SetFlagBit8 and unit_notifyabilitieschain.cpp's
//  NotifyAbilitiesOfMessage both read (QueryHandleField0x54,
//  Item/itemhandleresolve.h, resolving the SRefMiniValue-shaped handle
//  pair to a live ability object) - but this one doesn't walk a "next"
//  chain link off the resolved object the way NotifyAbilitiesOfMessage
//  does.  It re-reads the *same* +0x1DC slot every iteration: each
//  RemoveAbility call drops the front entry and (per the real,
//  unhooked 0x6F079CC0) rewrites +0x1DC to whatever used to be second,
//  so re-testing the same address is what walks the list - not a bug,
//  just a different removal shape from the read-only notify walk.
//
//  Reached from CUnit's own slot 3 (Method_0x0C) dispatch, case 852314
//  (unit_dispatch314.cpp) - the only caller in this tree.
//
//  0.683 IDENTICAL (14/19): the shipped body enters this loop's own guard
//  test once, at the top, and re-reaches the *same* instructions via the
//  bottom `jmp` back to it - one copy of the check, two control-flow
//  paths into it.  Every C++ loop shape tried (`for(;;)` with a trailing
//  `if (…) return;`, a plain `while`, an explicit `goto`, and the
//  guard-in-front `do`/`while` docs/msvc-vc8-idioms.md's own "A `for(;;)`
//  ... rotates" entry recommends) still has MSVC /O2 emit two physical
//  copies of the guard - one ahead of the loop, one at its bottom -
//  because the loop body has an early `return` in addition to its normal
//  fall-through exit, which none of that entry's own examples do. Past
//  the duplicated guard every instruction matches 1:1.
//============================================================================
#include "unit.h"
#include "../Item/itemhandleresolve.h"

struct SUnitAbilityHost
{
    void RemoveAbility(void* ability);   // 0x6F079CC0 (`retn 4`) -
                                          // unit_flagbit8thunks.cpp
    void RemoveAllAbilities();
};

void SUnitAbilityHost::RemoveAllAbilities()
{
    SOptionalHandleRef* ref = (SOptionalHandleRef*)((char*)this + 0x1DC);
    if (((unsigned int)ref->m_typeTag & ref->m_handle) != 0xFFFFFFFFu)
    {
        do
        {
            void* ability = QueryHandleField0x54(ref);
            if (!ability)
                return;

            RemoveAbility(ability);
        } while (((unsigned int)ref->m_typeTag & ref->m_handle) != 0xFFFFFFFFu);
    }
}
