//============================================================================
//  0x6F4AA370 - NIpse::CPrBehavior::DispatchIfEnabled, vtable slot 8. See
//  prbehavior.h's own comment: skip the call only when m_flags bit 2 is set
//  and bit 8 is clear; otherwise set bit 4 for the duration of the call (a
//  reentrancy guard) and clear it again after, dispatching through this
//  object's own vtable slot 17 (+0x44) - `_purecall` at this level
//  (purecallstub.cpp), so this only does something real once a concrete
//  derived class overrides that slot.
//
//  Hand-written indirect dispatch (CLAUDE.md's own vtable_dispatch_audit.py
//  section), the same reasoning Agent/agentrelationteardown.cpp's own
//  OnRegistrationRetired already gives: no real `class CPrBehavior : ...`
//  with virtuals is declared anywhere in this repo, so the call has to go
//  through the object's own vtable pointer by hand. Slot 17 (+0x44) is
//  this class's own confirmed docs/targets/vtables/CPrBehavior.txt entry.
//============================================================================
#include "prbehavior.h"

typedef void (__thiscall *CPrBehaviorSlot17Fn)(void*, int);

namespace NIpse {

void CPrBehavior::DispatchIfEnabled(int arg)
{
    if (!(m_flags & 2) || (m_flags & 8))
    {
        m_flags |= 4;

        CPrBehaviorSlot17Fn slot17 = (CPrBehaviorSlot17Fn)(*(void***)this)[0x44 / 4];
        slot17(this, arg);

        m_flags &= ~4u;
    }
}

}  // namespace NIpse
