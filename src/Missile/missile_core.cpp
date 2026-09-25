//============================================================================
//  The small CMissile vtable overrides that are single-purpose leaves too
//  small to earn their own translation unit: the FourCC constant, the
//  class name, GetRecordVersion's own additive constant, the two
//  "unregister as observer, then tail into the base non-virtually" leaves
//  (Deactivate/Method_0xA8), the slot-47 empty-override replacement, and
//  the slot-49 forward.  None of these six addresses appears in any
//  asm/*.md dump; each is a direct, instruction-by-instruction
//  transcription of its own agent_worktrees raw_asm.
//============================================================================
#include "missile.h"
#include "missile_thunderbolt.h"
#include "CFloat.h"

//  0x6F2BBA80 - {handle,typeTag} pair at this+0xA0/+0xA4 (misc_handle_
//  resolvers.cpp).  Re-declared here rather than pulled in through a
//  header because that file doesn't have one - same convention
//  missile_arrival.cpp already uses for this exact struct.
struct SHandleRefAt0xA0Owner { void* __thiscall GetField0x54IfSet(); };

//----------------------------------------------------------------------------
//  0x6F2BB740 - vtable slot 7.  A bare FourCC-shaped constant, same shape
//  as every other level of this family.
//----------------------------------------------------------------------------
unsigned int CMissile::GetAgileTypeId() const
{
    return 0x422D4D69u;
}

//----------------------------------------------------------------------------
//  0x6F0A42A0 - vtable slot 10, GetRecordVersion.  A direct (non-virtual)
//  call straight to the shared CAgent::GetRecordVersion leaf (agent.h,
//  "always 0") - not a chained CBullet::GetRecordVersion() (which itself
//  adds 11 over the same leaf) - plus fourteen at this level.
//----------------------------------------------------------------------------
int CMissile::GetRecordVersion(int context)
{
    return CAgent::GetRecordVersion(context) + 14;
}

//----------------------------------------------------------------------------
//  0x6F2BEC60 - vtable slot 13, Deactivate.  If the +0xA0 ref resolves,
//  unregister this object as its observer, then tail into
//  CBulletBase::Deactivate() directly (not virtually, and not through
//  CBullet's own inherited body - CBullet does not override this slot).
//----------------------------------------------------------------------------
void CMissile::Deactivate()
{
    void* target = ((SHandleRefAt0xA0Owner*)this)->GetField0x54IfSet();
    if (target)
        ((CMissileThunderBoltRefs*)target)->SetObserverRegistration(this, 0);

    CBulletBase::Deactivate();
}

//----------------------------------------------------------------------------
//  0x6F2BB6F0 - vtable slot 22.  "CMissile", the untruncated string per
//  agent_worktrees names.json.
//----------------------------------------------------------------------------
const char* CMissile::GetClassName()
{
    return "CMissile";
}

//----------------------------------------------------------------------------
//  0x6F2BD010 - vtable slot 42, Method_0xA8.  Same shape as Deactivate
//  above: unregister as the +0xA0 ref's observer if it resolves, then tail
//  into CBullet::Method_0xA8() directly (not virtually).
//----------------------------------------------------------------------------
void CMissile::Method_0xA8()
{
    void* target = ((SHandleRefAt0xA0Owner*)this)->GetField0x54IfSet();
    if (target)
        ((CMissileThunderBoltRefs*)target)->SetObserverRegistration(this, 0);

    CBullet::Method_0xA8();
}

//----------------------------------------------------------------------------
//  0x6F2BE8B0 - a deep, SEH-guarded impact-reconfirmation closure
//  (constructs its own SImpactContext, calls the same BuildImpactContext
//  OnArrival uses, re-runs the flags/unit-type gate, then dispatches
//  through the resolved unit's own vtable+0x120) - out of this session's
//  depth budget.  Naked redirect to the real, unhooked shipped body, same
//  convention CBullet.md's own Position-family leaves and OnArrival's own
//  BuildImpactContext use.
//----------------------------------------------------------------------------
//  Real signature confirmed against build/*.asm (verify.py): three stack
//  args, not two - `outParam` is a caller-owned scratch dword the deep
//  closure fills in (read back by nothing in *this* function, so its
//  contents are not asserted), pushed as `&outParam` ahead of `target`
//  and `param`.
struct SFieldA0ImpactRecheck
{
    void* __thiscall Recheck(void* outParam, void* target, const CFloat* param);
};

__declspec(naked) void* __thiscall
SFieldA0ImpactRecheck::Recheck(void*, void*, const CFloat*)
{
    __asm { mov eax, 0x6F2BE8B0 }
    __asm { jmp eax }
}

extern const CFloat g_slopeThreshold1;   // dword_6FAAE4F4 - missile_arrival.cpp

//----------------------------------------------------------------------------
//  0x6F2BECB0 - vtable slot 47, Method_0xBC.  Overrides the empty
//  `nullsub_1286` body every level from CBulletBase through CBullet left
//  untouched (0x6F2BB580): if the +0xA0 ref resolves, runs the deep
//  reconfirmation closure above against it, then tail-calls this object's
//  own Method_0xA8 (slot 42) - a genuine virtual redispatch on `this`
//  (`mov edx,[esi]; call [edx+0xA8]`), not the direct base calls
//  Deactivate/Method_0xA8 above make.
//----------------------------------------------------------------------------
void CMissile::Method_0xBC()
{
    void* target = ((SHandleRefAt0xA0Owner*)this)->GetField0x54IfSet();
    if (target)
    {
        void* outParam;
        ((SFieldA0ImpactRecheck*)this)->Recheck(&outParam, target, &g_slopeThreshold1);
    }

    this->Method_0xA8();
}

//----------------------------------------------------------------------------
//  0x6F2BBAA0 - vtable slot 49, GetImpactApplyFlag.  Overrides CBullet's
//  own bare-constant answer (0x6F051710, always 0) with a genuine forward:
//  if the +0xA0 ref resolves, tail-jumps into *that* object's own vtable
//  slot 0xB8 (a different, unrelated class's vtable - reached only by byte
//  offset, not this hierarchy's own OnTargetRefNotify); otherwise 0.  Both
//  paths agree on zero stack arguments, matching this slot's own fixed
//  contract everywhere else in the hierarchy.
//----------------------------------------------------------------------------
typedef int (__thiscall *VTableSlot0xB8Fn)(void* self);

int CMissile::GetImpactApplyFlag() const
{
    void* target = ((SHandleRefAt0xA0Owner*)this)->GetField0x54IfSet();
    if (!target)
        return 0;

    VTableSlot0xB8Fn fn = (VTableSlot0xB8Fn)(*(void***)target)[0xB8 / 4];
    return fn(target);
}
