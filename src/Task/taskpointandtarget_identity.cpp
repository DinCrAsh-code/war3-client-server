//============================================================================
//  CTaskPointAndTarget's two "what am I" vtable overrides - both
//  two-instruction leaves, clustered at 0x6F271Bxx right next to
//  CTaskPointAndTarget's own InstanceGenerator singleton getter
//  (0x6F271B30, own TU for the reason ctaskpointandtargetinstancegenerator.h
//  gives), so kept in their own TU here rather than pulled into that one -
//  same shape as taskpoint_identity.cpp/tasktarget_identity.cpp.
//============================================================================
#include "ctaskpointandtarget.h"

//----------------------------------------------------------------------------
//  0x6F271B20 - slot 22, GetClassName.  `retn 0`.
//----------------------------------------------------------------------------
const char* CTaskPointAndTarget::GetClassName()
{
    return "CTaskPointAndTarget";
}

//----------------------------------------------------------------------------
//  0x6F271B70 - slot 7, GetAgileTypeId.  `retn 0`.  Kept as the literal
//  shipped value rather than guessed at, same as CTaskPoint's own
//  0x74736B2E and CTaskTarget's own literal.
//----------------------------------------------------------------------------
unsigned int CTaskPointAndTarget::GetAgileTypeId() const
{
    return 0x746B2E74;
}
