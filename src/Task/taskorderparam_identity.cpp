//============================================================================
//  CTaskOrderParam's two "what am I" vtable overrides - both two-instruction
//  leaves, clustered at 0x6F2719xx right next to CTaskOrderParam's own
//  InstanceGenerator singleton getter (0x6F2719B0, own TU for the reason
//  ctaskorderparaminstancegenerator.h gives), so kept in their own TU here
//  rather than pulled into that one - same shape as CTaskParam's own
//  taskparam_identity.cpp.
//============================================================================
#include "taskorderparam.h"

//----------------------------------------------------------------------------
//  0x6F2719A0 - slot 22, GetClassName.  `retn 0`.
//----------------------------------------------------------------------------
const char* CTaskOrderParam::GetClassName()
{
    return "CTaskOrderParam";
}

//----------------------------------------------------------------------------
//  0x6F2719F0 - slot 7, GetAgileTypeId.  `retn 0`.  Literal 0x74736B4F -
//  same shape as CTask's own 0x7461736B and CTaskParam's own 0x74736B50:
//  not a clean readable MSB-first word here either, kept as the literal
//  value the shipped code returns rather than forcing a reading that is
//  not actually there.
//----------------------------------------------------------------------------
unsigned int CTaskOrderParam::GetAgileTypeId() const
{
    return 0x74736B4F;
}
