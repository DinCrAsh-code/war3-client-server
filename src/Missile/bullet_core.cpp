//============================================================================
//  The 0x6F2BB3xx/0x6F0517xx/0x6F051710 addresses that are single-purpose
//  leaves too small to earn their own translation unit: the FourCC
//  constant, the class name, GetRecordVersion's own additive constant, and
//  the new slot-49 gate.  No asm/*.md dump covers any of these four
//  addresses; each is a direct, instruction-by-instruction transcription
//  of its own agent_worktrees raw_asm.
//============================================================================
#include "bullet.h"

//----------------------------------------------------------------------------
//  0x6F2BB400 - vtable slot 7.  A bare FourCC-shaped constant, same shape
//  as every other level of this family (agent.h, agentwar3.h, war3image.h,
//  bulletbase.h all declare one at this slot).
//----------------------------------------------------------------------------
unsigned int CBullet::GetAgileTypeId() const
{
    return 0x2B776275u;
}

//----------------------------------------------------------------------------
//  0x6F0517B0 - vtable slot 10, GetRecordVersion.  A direct (non-virtual)
//  call straight to the shared CAgent::GetRecordVersion leaf (agent.h,
//  "always 0") - not a chained CBulletBase::GetRecordVersion() (which
//  itself adds 10 over the same leaf) - plus eleven at this level.
//----------------------------------------------------------------------------
int CBullet::GetRecordVersion(int context)
{
    return CAgent::GetRecordVersion(context) + 11;
}

//----------------------------------------------------------------------------
//  0x6F2BB3B0 - vtable slot 22.  "CBullet", the untruncated string per
//  agent_worktrees names.json.
//----------------------------------------------------------------------------
const char* CBullet::GetClassName()
{
    return "CBullet";
}

//----------------------------------------------------------------------------
//  0x6F051710 - vtable slot 49, new past CBulletBase's own 48.  Bare
//  `xor eax,eax`/`retn`, no stack args: an always-false gate at this
//  level, read by OnImpact (slot 48, bullet_impact.cpp) through this
//  object's own vtable.
//----------------------------------------------------------------------------
int CBullet::GetImpactApplyFlag() const
{
    return 0;
}
