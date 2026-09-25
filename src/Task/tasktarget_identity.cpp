//============================================================================
//  CTaskTarget's two "what am I" vtable overrides - both two-instruction
//  leaves, clustered at 0x6F271Bxx right next to CTask's own equivalents
//  (task_identity.cpp) and this class's own InstanceGenerator singleton
//  getter (0x6F271BB0, tasktargetinstancegenerator.cpp) - kept in their own
//  TU here, mirroring task_identity.cpp's own placement rule.
//============================================================================
#include "tasktarget.h"

//----------------------------------------------------------------------------
//  0x6F271BA0 - slot 22, GetClassName.  `retn 0`.
//----------------------------------------------------------------------------
const char* CTaskTarget::GetClassName()
{
    return "CTaskTarget";
}

//----------------------------------------------------------------------------
//  0x6F271BF0 - slot 7, GetAgileTypeId.  `retn 0`.  Literal 0x74736B74
//  decodes byte-for-byte (MSB first) as "tskt" - this family's "tsk" prefix
//  (CTask's own is "task") plus one differentiator letter, the same scheme
//  task_identity.cpp's own header comment gives for CTask.
//----------------------------------------------------------------------------
unsigned int CTaskTarget::GetAgileTypeId() const
{
    return 0x74736B74;   // 'tskt'
}
