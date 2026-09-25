//============================================================================
//  CTaskTarget2's two "what am I" vtable overrides - both two-instruction
//  leaves, clustered at 0x6F271Cxx right next to CTaskTarget's own
//  equivalents (tasktarget_identity.cpp) and this class's own
//  InstanceGenerator singleton getter (0x6F271C30,
//  tasktarget2instancegenerator.cpp) - kept in their own TU here, mirroring
//  tasktarget_identity.cpp's own placement rule.
//============================================================================
#include "tasktarget2.h"

//----------------------------------------------------------------------------
//  0x6F271C20 - slot 22, GetClassName.  `retn 0`.
//----------------------------------------------------------------------------
const char* CTaskTarget2::GetClassName()
{
    return "CTaskTarget2";
}

//----------------------------------------------------------------------------
//  0x6F271C70 - slot 7, GetAgileTypeId.  `retn 0`.  Literal 0x746B7432
//  decodes byte-for-byte (MSB first) as "tkt2" - this family's own scheme
//  (tasktarget_identity.cpp's own header comment: CTask's "task", CTaskTarget's
//  "tskt"), one differentiator character short here rather than a plain
//  "tskt"+"2" - read directly off the constant rather than assumed.
//----------------------------------------------------------------------------
unsigned int CTaskTarget2::GetAgileTypeId() const
{
    return 0x746B7432;   // 'tkt2'
}
