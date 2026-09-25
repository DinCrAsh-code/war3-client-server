//============================================================================
//  CBulletPath's two single-purpose leaves, same shape as
//  bulletbase_dispatch.cpp/missile_core.cpp's own. Neither address appears
//  in any asm/*.md dump; each is a direct, instruction-for-instruction
//  transcription of its own agent_worktrees raw_asm.
//============================================================================
#include "bulletpath.h"

//----------------------------------------------------------------------------
//  0x6F2BB480 - vtable slot 7.  A bare FourCC-shaped constant, same shape
//  as every other level of this family.
//----------------------------------------------------------------------------
unsigned int CBulletPath::GetAgileTypeId() const
{
    return 0x2B776270u;
}

//----------------------------------------------------------------------------
//  0x6F2BB430 - vtable slot 22.  "CBulletPath", the untruncated string per
//  agent_worktrees names.json.
//----------------------------------------------------------------------------
const char* CBulletPath::GetClassName()
{
    return "CBulletPath";
}
