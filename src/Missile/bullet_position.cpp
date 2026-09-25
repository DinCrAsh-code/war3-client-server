//============================================================================
//  The 0x6F0517xx/0x6F2BCCxx module: CBullet's own overrides of slots
//  CAgent/CBulletBase already gave a default body to, each layering one
//  extra step onto m_projectilePosition (its own +0x78 ProjectilePosition
//  sub-object, see bullet.h's own header comment) on top of the inherited
//  behaviour.
//
//  Two deep callees on m_projectilePosition itself (0x6F47A580, 0x6F47A860)
//  are not further reconstructed this session - genuinely new leaves that
//  belong to the Position family (Pathfinding/), not to CBullet, and both
//  several levels past this session's own depth budget (one does real
//  float-conversion arithmetic through two more unexamined callees,
//  `sub_6F496910`/`sub_6F6EF000`).  Kept as naked redirects to the real,
//  unhooked shipped addresses - the same pattern missile_apply.cpp's own
//  `SHitValidator::Validate` already uses for an equally out-of-depth
//  callee - so the call sites below are genuine calls, not guesses at a
//  body.
//============================================================================
#include "bullet.h"

//----------------------------------------------------------------------------
//  0x6F47A580 - a real, non-virtual member of ProjectilePositionTrack's own
//  Position family (ecx = &m_projectilePosition, one stack arg: a 3-float
//  out-buffer, retn 4).  Resolves this position's own tracked world
//  vector; not reconstructed here (see file header).
//----------------------------------------------------------------------------
struct SProjectilePositionVector
{
    void __thiscall GetWorldPosition(float* out3);
};
__declspec(naked) void __thiscall SProjectilePositionVector::GetWorldPosition(float*)
{
    __asm { mov eax, 0x6F47A580 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F47A860 - another real, non-virtual member of the same sub-object
//  (ecx = &m_projectilePosition, no stack args, bare `retn`).  Not
//  reconstructed here (see file header).
//----------------------------------------------------------------------------
struct SProjectilePositionNotify
{
    void __thiscall Notify();
};
__declspec(naked) void __thiscall SProjectilePositionNotify::Notify()
{
    __asm { mov eax, 0x6F47A860 }
    __asm { jmp eax }
}

//  ProjectilePositionTrack's own slot 6 (+0x18, `SubmitAttachRecord`) is
//  deliberately *not* spelled `virtual` on that class (projectilepositiontrack
//  .h's own header comment: a 3-int member cannot really override a
//  1-pointer-arg base virtual), so reaching it from here - a genuinely
//  different owner, at a genuinely indirect call site in the shipped code -
//  needs the same raw vtable-slot dispatch idiom bulletbase_core.cpp
//  already uses for its own unmodelled CAgentTimer slots, rather than an
//  ordinary (non-polymorphic) call to the member.
typedef int (__thiscall *SubmitAttachRecordFn)(void*, int, int, int);

//----------------------------------------------------------------------------
//  0x6F051750 - vtable slot 8, Method_0x20.  CAgentWar3/CWar3Image never
//  override this slot themselves (it is CAgent's own "always 0" default,
//  agent.h); CBullet is the first class in this chain to.  Direct
//  (non-virtual) call to the CAgent leaf, then forward {0, context, 0}
//  into m_projectilePosition's own SubmitAttachRecord through its own
//  vtable slot 6 - always 1.
//----------------------------------------------------------------------------
int CBullet::Method_0x20(int context)
{
    int base = CAgent::Method_0x20(context);
    ((SubmitAttachRecordFn)(*(void***)&m_projectilePosition)[6])(
        &m_projectilePosition, base, context, 0);
    return base + 1;
}

//----------------------------------------------------------------------------
//  0x6F051780 - vtable slot 9, Method_0x24.  Same shape one level up the
//  ancestor chain: CAgent::Method_0x24(context) (which itself virtually
//  redispatches to `this`'s own GetRecordVersion and discards the answer,
//  agent.h), then forward {0, context} into m_projectilePosition's own
//  AttachFromTable - a real override with a matching signature on that
//  class, so an ordinary virtual call reproduces the shipped indirect
//  dispatch exactly.  Always 1.
//----------------------------------------------------------------------------
int CBullet::Method_0x24(int context)
{
    int base = CAgent::Method_0x24(context);
    m_projectilePosition.AttachFromTable(base, (SIndexedHandleTable*)context);
    return base + 1;
}

//----------------------------------------------------------------------------
//  0x6F051720 - vtable slot 40, GetTrackedVector.  Overrides CBulletBase's
//  default "zero the out-buffer" (bulletbase.h) with a real resolve
//  through m_projectilePosition.
//----------------------------------------------------------------------------
void CBullet::GetTrackedVector(float* out3) const
{
    float tmp[3];
    ((SProjectilePositionVector*)&m_projectilePosition)->GetWorldPosition(tmp);
    out3[0] = tmp[0];
    out3[1] = tmp[1];
    out3[2] = tmp[2];
}

//----------------------------------------------------------------------------
//  0x6F2BCCC0 - vtable slot 42, Method_0xA8.  One extra step
//  (m_projectilePosition's own notify leaf) layered on top of
//  CBulletBase::Method_0xA8 (0x6F2BCBF0, bulletbase_slot42.cpp), reached
//  with a real tail-jump in the shipped code.
//----------------------------------------------------------------------------
void CBullet::Method_0xA8()
{
    ((SProjectilePositionNotify*)&m_projectilePosition)->Notify();
    CBulletBase::Method_0xA8();
}
