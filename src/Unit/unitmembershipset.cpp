//============================================================================
//  0x6F421E20 - SUnitMembershipSet::Contains: two instructions, `add ecx,
//  1E8h` / `jmp`, which is /O2's tail call for `return m_list.Contains(unit);`
//  on a member sub-object at +0x1E8.
//
//  A naked redirect in unit_playertable.cpp until now, and the comment
//  there had already read its shape correctly - including that its argument
//  count comes from the *callee's* `retn 4` and not from its own body,
//  which has no `retn` at all.
//
//  Own translation unit: the tail call has to stay a call.
//============================================================================
#include "unitmembership.h"

int __thiscall SUnitMembershipSet::Contains(void* unit)
{
    return m_list.Contains(unit);
}
