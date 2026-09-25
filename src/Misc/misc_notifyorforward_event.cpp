//============================================================================
//  0x6F2AB3E0 - notify or forward a target through a fixed event id
//  (0xD01A0), picked by a flag argument.  `this` in ecx, two stack
//  arguments (`retn 8`): the target object and the flag.
//
//  Reached by two entirely unrelated receivers in this tree - CUnit (via
//  CUnit::NotifyAbilityHostChanged, Unit/unit_abilityhostnotify.cpp, flag=1)
//  and CMissileThunderBoltRefs (via UpdateRef54,
//  Misc/cunit_agent8_batch2.cpp, flag=1) - both of which previously carried
//  their own, differently-named naked link-completion thunk at this same
//  address (the two-symbols-for-one-address defect docs/notes/verifier-
//  gate-link-failures.md warns about, avoided here the same way
//  observerpostevent.cpp's own header describes: one real body, every call
//  site now reaches it through this one shared, minimal receiver view).
//
//  Non-zero flag: a direct three-argument vtable slot 2 dispatch on `this`
//  with the event id doubled as both of the first two arguments and the
//  target as the third - not otherwise explained, but read directly off
//  the pushed operands (first push is the *last* stack parameter, per
//  ordinary cdecl-style argument order).  Zero flag: forward to
//  CObserver::PostEvent(0xD01A0, target) instead - a tail jump in the
//  shipped code, an ordinary call here (this build makes no attempt to
//  force MSVC's own tail-call decision).
//
//  Own translation unit: a homeless leaf with two unrelated receivers,
//  src/README.md's "Misc/ is a real answer, not a failure".
//============================================================================
#include "observer.h"

struct SEventNotifyOrForwardHost
{
    void NotifyOrForwardEvent(void* target, int flag);
};

typedef void (__thiscall *NotifySlot2Fn)(void*, int, int, void*);

void SEventNotifyOrForwardHost::NotifyOrForwardEvent(void* target, int flag)
{
    if (flag != 0)
    {
        ((NotifySlot2Fn)(*(void***)this)[2])(this, 0xD01A0, 0xD01A0, target);
        return;
    }

    ((CObserver*)this)->PostEvent(0xD01A0, target);
}
