//============================================================================
//  CMissileSpiderAttack's two single-purpose leaves. Neither address
//  appears in any asm/*.md dump; each is a direct, instruction-for-
//  instruction transcription of its own agent_worktrees raw_asm.
//============================================================================
#include "missilespiderattack.h"

//----------------------------------------------------------------------------
//  0x6F1D70D0 - vtable slot 7.  A bare FourCC-shaped constant - decodes as
//  ASCII "Mspa" (Missile Spider Attack), unlike its siblings' constants.
//----------------------------------------------------------------------------
unsigned int CMissileSpiderAttack::GetAgileTypeId() const
{
    return 0x4D737061u;
}

//----------------------------------------------------------------------------
//  0x6F1D7080 - vtable slot 22.  "CMissileSpiderAttack", the untruncated
//  string per agent_worktrees names.json.
//----------------------------------------------------------------------------
const char* CMissileSpiderAttack::GetClassName()
{
    return "CMissileSpiderAttack";
}
