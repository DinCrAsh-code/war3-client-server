//============================================================================
//  0x6F2C1B20 / 0x6F2BEC80 - CMissileThunderBoltRefs::ClearTargetRef/
//  ClearFieldA0Ref (see missile_thunderbolt.h).  Own TU, separate from
//  SetTargetRef/SetFieldA0Ref: the target listing shows a real `call` into
//  these from their Set* counterparts rather than inlining them, despite
//  being small - the shipped build compiled them separately.
//============================================================================
#include "missile_thunderbolt.h"

struct SHandleRefAt0x2COwner { void* __thiscall GetField0x54IfSet(); };
struct SHandleRefAt0xA0Owner { void* __thiscall GetField0x54IfSet(); };

void __thiscall CMissileThunderBoltRefs::ClearTargetRef()
{
    void* target = ((SHandleRefAt0x2COwner*)this)->GetField0x54IfSet();
    if (target)
        ((CMissileThunderBoltRefs*)target)->SetObserverRegistration(this, 0);

    *(unsigned int*)((char*)this + 0x30) = 0xFFFFFFFF;
    *(unsigned int*)((char*)this + 0x2C) = 0xFFFFFFFF;
}

void __thiscall CMissileThunderBoltRefs::ClearFieldA0Ref()
{
    void* target = ((SHandleRefAt0xA0Owner*)this)->GetField0x54IfSet();
    if (target)
        ((CMissileThunderBoltRefs*)target)->SetObserverRegistration(this, 0);

    *(unsigned int*)((char*)this + 0xA4) = 0xFFFFFFFF;
    *(unsigned int*)((char*)this + 0xA0) = 0xFFFFFFFF;
}
