//============================================================================
//  See missile_thunderbolt.h.  Calls into ClearTargetRef/ClearFieldA0Ref
//  (missile_thunderbolt_clear.cpp) and SetObserverRegistration
//  (missile_thunderbolt_register.cpp) for real rather than inlining them -
//  each kept in its own TU for exactly that reason.
//============================================================================
#include "missile_thunderbolt.h"

struct SOptionalHandleRefResolver : SOptionalHandleRef
{
    SOptionalHandleRef& ResolveChained(const void* ref);
};

//----------------------------------------------------------------------------
//  0x6F2C2AC0 - ClearTargetRef(), then adopt `newTarget` if given.
//----------------------------------------------------------------------------
void __thiscall CMissileThunderBoltRefs::SetTargetRef(void* newTarget)
{
    ClearTargetRef();

    if (newTarget)
    {
        ((CMissileThunderBoltRefs*)newTarget)->SetObserverRegistration(this, 1);
        ((SOptionalHandleRefResolver*)((char*)this + 0x2C))->ResolveChained(newTarget);
    }
}

//----------------------------------------------------------------------------
//  0x6F2C1B50 - same shape as SetTargetRef, on the +0xA0 slot.
//----------------------------------------------------------------------------
void __thiscall CMissileThunderBoltRefs::SetFieldA0Ref(void* newValue)
{
    ClearFieldA0Ref();

    if (newValue)
    {
        ((CMissileThunderBoltRefs*)newValue)->SetObserverRegistration(this, 1);
        ((SOptionalHandleRefResolver*)((char*)this + 0xA0))->ResolveChained(newValue);
    }
}
