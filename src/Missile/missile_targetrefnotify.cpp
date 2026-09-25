//============================================================================
//  0x6F2C1B80 - CMissile's vtable slot 46, OnTargetRefNotify.  Overrides
//  CBullet's inherited CBulletBase::OnTargetRefNotify (0x6F2BE7E0, still
//  CBullet's own slot 46 - CBullet does not touch this slot itself).
//
//  Read address-by-address off the raw disassembly (SEH frame elided, same
//  __except_handler4 shape docs/msvc-vc8-idioms.md already documents this
//  toolchain cannot emit): calls the base handler directly first, then
//  type-checks both the notification's own +0x0C reference and this
//  missile's own +0xA0 ref as `SCheckedWidgetSlot` (agenttypedslots.h,
//  0x6F0382B0's own constructor - the same "narrow to a required agile
//  type, holding a counted reference for the comparison" idiom
//  missile_arrival.cpp already uses with SCheckedUnitSlot).  A subtlety
//  worth naming: MSVC reuses the incoming `msg` parameter's own stack slot
//  as the first SCheckedWidgetSlot local once `msg` itself is no longer
//  needed (the two are the same size, one pointer, and nothing after this
//  point reads the raw `msg` value again) - not modelled here as a literal
//  slot reuse, since an ordinary named local reproduces the same value
//  flow without depending on the compiler picking the identical stack
//  slot.
//
//  If the two checked references are the *same* object (the just-notified
//  reference is this missile's own +0xA0 target), a deeper, SEH-guarded
//  reconfirmation (0x6F2BECE0, out of this session's depth budget) decides
//  whether the +0xA0 ref is still valid; if not, it is cleared.
//============================================================================
#include "missile.h"
#include "missile_thunderbolt.h"
#include "agenttypedslots.h"

//  0x6F2BBA80 - same struct missile_core.cpp/missile_arrival.cpp already
//  redeclare locally (misc_handle_resolvers.cpp has no header of its own).
struct SHandleRefAt0xA0Owner { void* __thiscall GetField0x54IfSet(); };

//  0x6F2BECE0 - deep, SEH-guarded closure that re-validates a candidate
//  against this missile's own +0x2C/+0x30 AND-masked comparison field and a
//  fresh FourCC-shaped type check, then (on a specific failure combination)
//  fires an observer-forwarding call through the candidate's own
//  vtable+0xB8 - out of this session's depth budget.  Naked redirect to
//  the real, unhooked shipped body, same convention this class's other
//  out-of-depth leaves use (missile_core.cpp).
struct SFieldA0ReconfirmOwner
{
    int __thiscall Reconfirm(void* candidate);
};

__declspec(naked) int __thiscall
SFieldA0ReconfirmOwner::Reconfirm(void*)
{
    __asm { mov eax, 0x6F2BECE0 }
    __asm { jmp eax }
}

void CMissile::OnTargetRefNotify(const SBulletDispatchMsg* msg)
{
    CBulletBase::OnTargetRefNotify(msg);

    CAgent* notified = *(CAgent**)((char*)msg + 0x0C);
    SCheckedWidgetSlot notifiedWidget(notified);

    CAgent* fieldA0 = (CAgent*)((SHandleRefAt0xA0Owner*)this)->GetField0x54IfSet();
    SCheckedWidgetSlot fieldA0Widget(fieldA0);

    if (notifiedWidget.m_value == fieldA0Widget.m_value)
    {
        if (!((SFieldA0ReconfirmOwner*)this)->Reconfirm(notifiedWidget.m_value))
            ((CMissileThunderBoltRefs*)this)->ClearFieldA0Ref();
    }
}
