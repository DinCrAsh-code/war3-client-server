//============================================================================
//  CTaskParam's two "what am I" vtable overrides - both two-instruction
//  leaves, clustered at 0x6F27187x/0x6F2718xx right next to CTaskParam's own
//  InstanceGenerator singleton getter (0x6F271830, own TU for the reason
//  ctaskparaminstancegenerator.h gives), so kept in their own TU here rather
//  than pulled into that one - same shape as CTask's own task_identity.cpp.
//============================================================================
#include "taskparam.h"

//----------------------------------------------------------------------------
//  0x6F271820 - slot 22, GetClassName.  `retn 0`.
//----------------------------------------------------------------------------
const char* CTaskParam::GetClassName()
{
    return "CTaskParam";
}

//----------------------------------------------------------------------------
//  0x6F271870 - slot 7, GetAgileTypeId.  `retn 0`.  Literal 0x74736B50 -
//  unlike CTask's own 0x7461736B ("task", MSB-first ASCII), this one does
//  not decode as a clean readable word ('t','s','k','P' MSB-first); kept as
//  the literal value the shipped code returns rather than forcing a reading
//  that is not actually there.
//----------------------------------------------------------------------------
unsigned int CTaskParam::GetAgileTypeId() const
{
    return 0x74736B50;
}
