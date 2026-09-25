//============================================================================
//  0x6F40F400 - CPlayerWar3::Method_0x24, vtable slot 9 (+0x24).  Seeds a
//  counter from the base class's own default (CAgent::Method_0x24, which
//  always returns 0 - agentdefaults.cpp), then walks the 26-entry
//  AbilityEntry array at +0x40 (playerwar3.h), calling each element's own
//  vtable slot 3 as `Method(counter, context)` and incrementing the
//  counter once per element.  The final counter (== 26, since the base
//  contributes 0) is the return value.
//
//  The push order pins the argument order: `push ebp` (context) happens
//  before `push edi` (counter) at each iteration, so the counter - the
//  later push - is the callee's first stack argument and context is its
//  second.
//============================================================================
#include "playerwar3.h"

typedef int (__thiscall *AbilityEntryMethod0xCFn)(void* self, int counter, int context);

int CPlayerWar3::Method_0x24(int context)
{
    int count = CAgent::Method_0x24(context);

    for (int i = 0; i < 0x1A; ++i)
    {
        AbilityEntry* entry = &m_abilityEntries[i];
        AbilityEntryMethod0xCFn fn = (AbilityEntryMethod0xCFn)entry->m_vtbl[3];
        fn(entry, count, context);
        ++count;
    }

    return count;
}
