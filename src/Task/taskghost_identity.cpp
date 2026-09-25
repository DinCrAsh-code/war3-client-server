//============================================================================
//  CTaskGhost's two "what am I" vtable overrides - both two-instruction
//  leaves, clustered at 0x6F271Cxx right next to CTaskGhost's own
//  InstanceGenerator singleton getter (0x6F271CB0, own TU for the reason
//  ctaskghostinstancegenerator.h gives), so kept in their own TU here
//  rather than pulled into that one - same shape as CTaskOrderParam's own
//  taskorderparam_identity.cpp.
//============================================================================
#include "taskghost.h"

//----------------------------------------------------------------------------
//  0x6F271CA0 - slot 22, GetClassName.  `retn 0`.
//----------------------------------------------------------------------------
const char* CTaskGhost::GetClassName()
{
    return "CTaskGhost";
}

//----------------------------------------------------------------------------
//  0x6F271CF0 - slot 7, GetAgileTypeId.  `retn 0`.  Literal 0x74736B67 -
//  same shape as CTask's own 0x7461736B, CTaskParam's own 0x74736B50 and
//  CTaskOrderParam's own 0x74736B4F: not a clean readable MSB-first word
//  here either, kept as the literal value the shipped code returns rather
//  than forcing a reading that is not actually there.
//----------------------------------------------------------------------------
unsigned int CTaskGhost::GetAgileTypeId() const
{
    return 0x74736B67;
}
