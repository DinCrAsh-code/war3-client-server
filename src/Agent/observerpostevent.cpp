//============================================================================
//  0x6F62A570 - CObserver::PostEvent: hand an event to this observer's own
//  registration list, if it has one.
//
//  Five instructions, and it was a thunk *twice* - once as
//  `CObserver::PostEvent` (unreconstructed_thunks.cpp) and once as
//  `SObserverUnregisterTarget::UnregisterObserverEvent`
//  (missile_thunderbolt_register.cpp), which is the
//  two-symbols-for-one-address defect
//  docs/notes/verifier-gate-link-failures.md warns about: two naked bodies
//  at one address, and whichever the linker picked is the one every caller
//  got.  Both are gone; this is the single body, and both call sites now
//  reach it.
//
//  The guard is the observer's own +0x08 resource - the same field
//  CObserver::EnsureResource builds and the destructor releases
//  (observer.h) - and the forward is a tail jump on the same frame, so the
//  callee takes the event and the listener in this function's own two stack
//  slots.
//
//  Own translation unit: two callers reach it with real calls and it makes
//  a real tail jump of its own.
//============================================================================
#include "observer.h"
#include "observereventreg.h"

//  0x6F62A000 - the registration list's own event walk, reconstructed for
//  real as CObserverEventList::Dispatch (Agent/observereventreg.cpp/.h) -
//  `m_resource` really is a CObserverEventList (that file's own header
//  note), not a locally-invented "SObserverResourceEvents".
void CObserver::PostEvent(unsigned int event, void* target)
{
    CObserverEventList* events = (CObserverEventList*)m_resource;
    if (events != 0)
        return events->Dispatch(event, target);
}
