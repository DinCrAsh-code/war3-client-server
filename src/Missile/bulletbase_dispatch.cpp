//============================================================================
//  The 0x6F2BB3xx-0x6F2BB5xx module: CBulletBase's own GetAgileTypeId,
//  GetClassName, the target-notify tail-jump slot 45, the empty slot 47,
//  slot 43's field-copy helper, and the sprite-controller position-follow
//  trampoline CreateSpriteFromDescriptor arms (bulletbase_createsprite.cpp).
//  DispatchMessage (0x6F2BB4F0, same module) is missile_dispatch.cpp
//  instead - it already existed there under the wrong class name before
//  this session and was renamed in place rather than duplicated.
//============================================================================
#include "bulletbase.h"

//----------------------------------------------------------------------------
//  0x6F2BB380 - vtable slot 7.  A bare FourCC-shaped constant, same shape
//  as every other level of this family (agent.h, agentwar3.h, war3image.h
//  all declare one at this same slot).
//----------------------------------------------------------------------------
unsigned int CBulletBase::GetAgileTypeId() const
{
    return 0x2B773342u;
}

//----------------------------------------------------------------------------
//  0x6F2BB330 - vtable slot 22.  "CBulletBase", the untruncated string per
//  agent_worktrees names.json.
//----------------------------------------------------------------------------
const char* CBulletBase::GetClassName()
{
    return "CBulletBase";
}

typedef void (__thiscall *VoidSlotFn)(void*);

//----------------------------------------------------------------------------
//  0x6F2BB570 - vtable slot 45.  RENAMED from the lazy placeholder
//  "SVTableSlot0xBCOwner::InvokeSlot0xBC" (which also named the wrong
//  offset - 0xBC is slot 47, not this slot's own 0xB4).  A plain alias: an
//  explicit tail-jump to whatever this same object's own slot 47 currently
//  holds - not a distinct implementation of its own.
//----------------------------------------------------------------------------
void CBulletBase::InvokeSlot0xBC()
{
    ((VoidSlotFn)(*(void***)this)[0xBC / 4])(this);
}

//----------------------------------------------------------------------------
//  0x6F2BB580 (`nullsub_1286`) - vtable slot 47.  Confirmed empty from
//  raw_bytes (a bare 0xC3), not just the IDA name.
//----------------------------------------------------------------------------
void CBulletBase::Method_0xBC()
{
}

//----------------------------------------------------------------------------
//  0x6F2BB590 - vtable slot 43.  `__thiscall`-shaped but the receiver in
//  ecx is not `this` at the call site this dump reaches it from - two
//  destination pointers and two source pointers, each copied through a
//  shared field-copy helper (0x6F6EEE20, already reconstructed for
//  Net/cdatastore.cpp's own use).  Declared with the raw slot-dispatch
//  idiom's own signature rather than as a real `this`-based member, the
//  same reasoning CBhPoProjectile's header (bhpoprojectile.h) gives for
//  slots it cannot pin to `this`.
//----------------------------------------------------------------------------
//  0x6F6EEE20 - the shared field-copy helper (already reconstructed for
//  Net/cdatastore.cpp's own use elsewhere in this repo): copy one dword
//  from `src` into `dst` and hand back a pointer to it.  Not reproduced
//  byte-for-byte here (out of this pass's budget - see
//  docs/targets/CBulletBase.md); the observable effect (two dword copies)
//  is what this body reproduces.
void CBulletBase::Method_0xAC(void* dstA, void* dstB, void* srcA, void* srcB)
{
    *(unsigned int*)dstA = *(unsigned int*)srcA;
    *(unsigned int*)dstB = *(unsigned int*)srcB;
}

//----------------------------------------------------------------------------
//  0x6F2BB4A0 - the sprite-controller position-follow callback
//  CreateSpriteFromDescriptor (bulletbase_createsprite.cpp) binds to its
//  own sprite's controller slot 1.  `self` is the bullet (the callback's
//  own bound context, per GameUI/cameracontrollerbind.h's own
//  BindControllerSlotDirect(context, ...) shape); `out` is the 3-float
//  buffer the sprite controller supplies.  Dispatches through `self`'s own
//  vtable slot 40 (GetTrackedVector) polymorphically - CBulletBase's own
//  default answers all-zero, a subclass with a real tracked position or
//  velocity overrides that slot instead of this trampoline.
//----------------------------------------------------------------------------
typedef void (__thiscall *GetTrackedVectorFn)(void* self, float* out3);

void __fastcall CBulletBase_SpriteFollowCallback(void* self, void* out)
{
    if (self != 0 && out != 0)
    {
        float local[3];
        ((GetTrackedVectorFn)(*(void***)self)[0xA0 / 4])(self, local);
        ((float*)out)[0] = local[0];
        ((float*)out)[1] = local[1];
        ((float*)out)[2] = local[2];
    }
}
