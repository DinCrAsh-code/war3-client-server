//============================================================================
//  CTaskPoint2's two "what am I" vtable overrides - both two-instruction
//  leaves, clustered at 0x6F271Axx right next to CTaskPoint2's own
//  InstanceGenerator singleton getter (0x6F271AB0,
//  ctaskpoint2instancegenerator.h), so kept in their own TU here rather
//  than pulled into that one - same shape as taskpoint_identity.cpp.
//============================================================================
#include "taskpoint2.h"

//----------------------------------------------------------------------------
//  0x6F271AA0 - slot 22, GetClassName.  `retn 0`.
//----------------------------------------------------------------------------
const char* CTaskPoint2::GetClassName()
{
    return "CTaskPoint2";
}

//----------------------------------------------------------------------------
//  0x6F271AF0 - slot 7, GetAgileTypeId.  `retn 0`.  Literal shipped value,
//  same reasoning as CTaskPoint's own (taskpoint_identity.cpp) - does not
//  decode as a clean four-character tag either way round.
//----------------------------------------------------------------------------
unsigned int CTaskPoint2::GetAgileTypeId() const
{
    return 0x746B2E32;
}
