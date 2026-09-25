//============================================================================
//  CTaskPoint's two "what am I" vtable overrides - both two-instruction
//  leaves, clustered at 0x6F271Axx right next to CTaskPoint's own
//  InstanceGenerator singleton getter (0x6F271A30, own TU for the reason
//  ctaskpointinstancegenerator.h gives), so kept in their own TU here rather
//  than pulled into that one - same shape as task_identity.cpp.
//============================================================================
#include "taskpoint.h"

//----------------------------------------------------------------------------
//  0x6F271A20 - slot 22, GetClassName.  `retn 0`.
//----------------------------------------------------------------------------
const char* CTaskPoint::GetClassName()
{
    return "CTaskPoint";
}

//----------------------------------------------------------------------------
//  0x6F271A70 - slot 7, GetAgileTypeId.  `retn 0`.  Unlike CTask's own
//  0x7461736B ("task", a clean MSB-first FourCC), this literal
//  (0x74736B2E) does not decode as a clean four-character tag either way
//  round - kept as the literal shipped value rather than guessed at.
//----------------------------------------------------------------------------
unsigned int CTaskPoint::GetAgileTypeId() const
{
    return 0x74736B2E;
}
