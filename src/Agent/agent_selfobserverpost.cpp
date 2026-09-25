//============================================================================
//  0x6F471A40 - CAgent::PostEventToSelfObserver.  See agent.h.
//
//  The whole shipped function is `83 C1 14` / `E9 <rel32>`: add 0x14 to
//  `this` and tail-jump into CObserver::PostEvent (0x6F62A570,
//  observerpostevent.cpp, `retn 8`, confirmed from its own shipped bytes
//  `mov ecx,[ecx+8] / test ecx,ecx / jz / jmp 0x6F62A000 / retn 8`).  Both
//  stack words travel through untouched and the callee cleans them.
//
//  That +0x14 is CAgent's own *embedded* CObserver (agent.h), not the base
//  subobject at offset 0 - which is why this exists at all rather than the
//  caller simply calling PostEvent: a member sub-object needs an adjustor,
//  a base does not.  Its neighbour SModifierSubject::AddValueListener
//  (0x6F471A30, misc_handle_lookups.cpp) is the same shape onto the same
//  sub-object, through a vtable slot instead of a direct call.
//
//  Not in any dump, so not scored - see agent.h for where the bytes came
//  from.  Own translation unit: it must reach CObserver::PostEvent with a
//  real call, and its caller must reach it with one.
//============================================================================
#include "agent.h"
#include "observer.h"

void CAgent::PostEventToSelfObserver(unsigned int event, void* target)
{
    ((CObserver*)m_selfObserverStorage)->PostEvent(event, target);
}
