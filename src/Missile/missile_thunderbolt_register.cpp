//============================================================================
//  0x6F2AB2D0 - CMissileThunderBoltRefs::SetObserverRegistration (see
//  missile_thunderbolt.h).  Own TU, separate from SetTargetRef/
//  SetFieldA0Ref (missile_thunderbolt.cpp): the target listing shows a
//  real `call` into this from both rather than inlining it.
//============================================================================
#include "missile_thunderbolt.h"
#include "observer.h"

//  0x6F62A570 is CObserver::PostEvent (Agent/observerpostevent.cpp), now
//  reconstructed.  This file carried its own naked body for that address
//  under a made-up class name, which was the second of two symbols for one
//  function - the defect docs/notes/verifier-gate-link-failures.md
//  describes, and one the linker resolves arbitrarily.  Declared through
//  observer.h and called by its real name.

typedef void (__thiscall *VTableSlot2RegisterFn)(void* self, unsigned int a, unsigned int b, void* listener);

void __thiscall CMissileThunderBoltRefs::SetObserverRegistration(void* listener, int add)
{
    if (add)
    {
        VTableSlot2RegisterFn fn = (VTableSlot2RegisterFn)(*(void***)this)[2];
        fn(this, 0xD01A4, 0xD01A4, listener);
        return;
    }

    ((CObserver*)this)->PostEvent(0xD01A4, listener);
}
