//============================================================================
//  0x6F2BCBF0 - CBulletBase's vtable slot 42.  Plays this object's own
//  animation (the raw vtable-slot-35 dispatch shape, not the modelled
//  three-argument PlayAnimation in war3image.h - this call site only ever
//  pushes two stack dwords, one fewer than that slot's own general
//  signature takes, so it is dispatched by raw offset here rather than
//  forced through a mismatched virtual call), looks up the "Misc"/
//  "BulletDeathTime" config value, arms the embedded timer to re-dispatch
//  message id 0xD019F on `this` after that delay, and stops observing
//  whatever the target ref currently resolves to - the same unregister
//  tail Deactivate (bulletbase_deactivate.cpp) already uses.  Reads like
//  "begin this bullet's death sequence": play the death animation, and
//  schedule the eventual removal message.
//
//  CORRECTION: the original pass here invented an `SConfigLookupResult`/
//  `Lookup` member-call model for 0x6F009EE0 and a second, differently-
//  named `SArmableTimer::ArmDispatch` naked body for 0x6F4778F0 - both
//  addresses were already real, correctly-modelled reconstructions
//  elsewhere in this repo the whole time (`GetConfigFloat`,
//  Config/configfloat.cpp; `SAgentTimerArm::Arm`, declared-not-defined
//  the same way in Item/item_armdroptimer.cpp, real body in
//  Item/item_remove.cpp), and this file's own guessed `__thiscall`
//  signature for the "lookup" call was outright ABI-wrong (three stack
//  args, `retn 12`) against the real body's own `retn 8` - the build
//  only failed to link because of it. Fixed to call the two existing
//  reconstructions directly, the identical shape item_remove.cpp's own
//  `CItem::RemoveFromWorld` already uses for `m_timer1`/`EffectDeathTime`.
//============================================================================
#include "bulletbase.h"
#include "gamecontext.h"        // GetConfigFloat
#include "itemhandleresolve.h"
#include "missile_thunderbolt.h"

typedef float (__thiscall *PlayAnimSlotFn)(void*, unsigned int, int);

//  0x6F4778F0 - declared, not defined, here; Item/item_remove.cpp's own
//  naked body is the one definition this links against (see its own
//  file header for why: an unreproducible __except_handler4-shaped SEH
//  frame, docs/msvc-vc8-idioms.md).
struct SAgentTimerArm
{
    void Arm(const CFloat* delay, int eventId, void* subject, int a, int b);
};

void CBulletBase::Method_0xA8()
{
    ((PlayAnimSlotFn)(*(void***)this)[0x8C / 4])(this, 1, 0);

    CFloat delay = GetConfigFloat("Misc", "BulletDeathTime", 0);
    ((SAgentTimerArm*)&m_timer)->Arm(&delay, 0xD019F, this, 0, 0);

    if ((m_targetTypeTag & (int)m_targetHandle) != -1)
    {
        void* target = QueryHandleField0x54((SOptionalHandleRef*)&m_targetHandle);
        if (target)
            ((CMissileThunderBoltRefs*)target)->SetObserverRegistration(this, 0);
    }
}
