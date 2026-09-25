//============================================================================
//  0x6F2BE7B0 - CBulletBase's vtable slot 13, Deactivate.  Base
//  Deactivate(), stop the embedded timer, and - if the target ref still
//  resolves - unregister as its observer without touching the ref itself
//  (that is ClearTargetRef's job, missile_thunderbolt.h, not this one's).
//============================================================================
#include "bulletbase.h"
#include "itemhandleresolve.h"
#include "missile_thunderbolt.h"

void CBulletBase::Deactivate()
{
    CWar3Image::Deactivate();
    m_timer.Cancel();

    //  0x6F2BC9F0's own body (Misc/misc_handle_resolvers.cpp), inlined
    //  here rather than shared across a TU boundary: resolve the +0x2C
    //  target ref if it is set.
    if ((m_targetTypeTag & (int)m_targetHandle) != -1)
    {
        void* target = QueryHandleField0x54(
            (SOptionalHandleRef*)&m_targetHandle);
        if (target)
            ((CMissileThunderBoltRefs*)target)->SetObserverRegistration(this, 0);
    }
}
