//============================================================================
//  CMissilePath's two single-purpose leaves. Neither address appears in
//  any asm/*.md dump; each is a direct, instruction-for-instruction
//  transcription of its own agent_worktrees raw_asm.
//============================================================================
#include "missilepath.h"

//----------------------------------------------------------------------------
//  0x6F2BB7C0 - vtable slot 7.  A bare FourCC-shaped constant.
//----------------------------------------------------------------------------
unsigned int CMissilePath::GetAgileTypeId() const
{
    return 0x422D4D70u;
}

//----------------------------------------------------------------------------
//  0x6F2BB770 - vtable slot 22.  "CMissilePath", the untruncated string per
//  agent_worktrees names.json.
//----------------------------------------------------------------------------
const char* CMissilePath::GetClassName()
{
    return "CMissilePath";
}
