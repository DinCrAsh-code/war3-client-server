//============================================================================
//  CMissileThunderBolt's own handle-ref pair management and observer
//  registration, feeding into the confirmed "deal damage" call
//  (sub_6F2C2AF0, see missile_dealdamage.cpp) - CMissileThunderBolt impact
//  pipeline, docs/targets/cmissilethunderbolt_impact_pipeline.md.
//
//  The missile carries two independent {handle,typeTag} reference slots
//  (SOptionalHandleRef, itemhandleresolve.h): +0x2C ("target", read by
//  QueryHandleField0x54IfSet/sub_6F021A00 - handleref_field54_guarded.cpp -
//  and by SHandleRefAt0x2COwner - misc_handle_resolvers.cpp) and +0xA0
//  (read by SHandleRefAt0xA0Owner, same file). Both slots are always
//  managed in the same clear-then-set pattern, and both register/
//  unregister the missile as an observer on whatever they point at
//  through event code 0xD01A4 (852388) - the same case sub_6F2BB4F0's own
//  dispatch switch (missile_dispatch.cpp) recognises as its vtable+0xB8
//  case, confirming the two are the same observer-event mechanism.
//============================================================================
#ifndef MISSILE_THUNDERBOLT_H
#define MISSILE_THUNDERBOLT_H

#include "itemhandleresolve.h"
#include "CFloat.h"

//----------------------------------------------------------------------------
//  The reference-counted object the missile's +0xA0 slot resolves to.  Only
//  what this call tree touches is named: the vtable (whose slot 0 is the
//  release/destroy entry) and the counter at +4.
//
//  Declared here rather than in missile_arrival.cpp because its two query
//  methods are defined in refcountedagent_queries.cpp - two files, one
//  class declaration.
//----------------------------------------------------------------------------
struct SRefCountedAgent
{
    void* m_vtable;     // +0x00
    int   m_refCount;   // +0x04

    //  0x6F284EE0 - `retn 0Ch`, three stack arguments.  Asked with
    //  (data, 0, 4) and its answer gates the hit.
    int __thiscall QueryHitAllowed(void* data, int a, int b);

    //  0x6F0774E0 - `retn 4`.  Asked with the impact context, and its
    //  answer is the last gate before the hit.
    int __thiscall QueryContextAllows(void* context);
};

//  The 3-float impact point DealDamage (missile_dealdamage.cpp) builds
//  from the resolved world position's x/y and its own computed z, handed
//  to Apply (missile_apply.cpp) - see that file for how sub_6F2BCC50's own
//  body confirms this shape.
struct SImpactPoint { CFloat x, y, z; };

class CMissileThunderBoltRefs
{
public:
    //  0x6F2AB2D0 - register (`add` nonzero) or unregister (`add` zero)
    //  `listener` as an observer of event 0xD01A4 on `this`.  Not a member
    //  of CMissileThunderBolt itself - `this` here is the referenced
    //  object (the old/new target), called through its own vtable slot 2.
    void __thiscall SetObserverRegistration(void* listener, int add);

    //  0x6F2AB3E0 - CUnit's own vtable BFS closure (cunit_agent8_worklist
    //  .md) reached this address called with the identical (listener, add)
    //  argument shape right before SetObserverRegistration above, on the
    //  same receiver.  Reconstructed for real now as a standalone leaf,
    //  SEventNotifyOrForwardHost::NotifyOrForwardEvent
    //  (Misc/misc_notifyorforward_event.cpp, EXACT) - called through that
    //  shared struct at its own call site (Misc/cunit_agent8_batch2.cpp)
    //  rather than as a member here.

    //  0x6F2C1B20 - release the +0x2C ref: if it currently resolves,
    //  unregister as its observer, then reset the {handle,typeTag} pair to
    //  the {-1,-1} "unset" sentinel.
    void __thiscall ClearTargetRef();

    //  0x6F2C2AC0 - ClearTargetRef(), then if `newTarget` is non-null,
    //  register as its observer and resolve+store its {handle,typeTag}
    //  into the +0x2C slot (SOptionalHandleRefResolver::ResolveChained).
    void __thiscall SetTargetRef(void* newTarget);

    //  0x6F2BEC80 - same shape as ClearTargetRef, on the +0xA0 slot.
    void __thiscall ClearFieldA0Ref();

    //  0x6F2C1B50 - same shape as SetTargetRef, on the +0xA0 slot.
    void __thiscall SetFieldA0Ref(void* newValue);

    //  0x6F2C42D0 - the arrival/impact handler, pipeline frame [0]
    //  (missile_arrival.cpp).  Moved to `CMissile::OnArrival` (missile.h,
    //  vtable slot 45) this session: it is reached through the missile's
    //  own vtable slot 0xB4, which makes it a genuine CMissile override of
    //  CBulletBase::InvokeSlot0xBC, not a method of this organisational
    //  grouping struct - the previous name pre-dated knowing that.  Gates
    //  on the flags word at +0x20 and, when every gate passes, sets the
    //  "damage dealt" bit and calls DealDamage below.

    //  0x6F2C2AF0 - the actual "deal damage" call (missile_dealdamage.cpp)
    //  - best-effort, not yet EXACT/IDENTICAL, see its own file header and
    //  BEHAVIOUR entry.
    void __thiscall DealDamage(void* target, void* data);

    //  0x6F2BCC50 - the gated apply step DealDamage hands off to
    //  (missile_apply.cpp) - best-effort, not yet verified against a
    //  build, see its own file header.
    void __thiscall Apply(void* arg0, SImpactPoint* point, int flag);
};

#endif
