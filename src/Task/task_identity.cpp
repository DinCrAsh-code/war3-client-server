//============================================================================
//  CTask's two "what am I" vtable overrides - both two-instruction leaves,
//  clustered at 0x6F2717Ax/0x6F2717Fx right next to CTask's own
//  InstanceGenerator singleton getter (0x6F2717B0, own TU for the reason
//  ctaskinstancegenerator.h gives), so kept in their own TU here rather
//  than pulled into that one.
//============================================================================
#include "task.h"

//----------------------------------------------------------------------------
//  0x6F2717A0 - slot 22, GetClassName.  `retn 0`.
//----------------------------------------------------------------------------
const char* CTask::GetClassName()
{
    return "CTask";
}

//----------------------------------------------------------------------------
//  0x6F2717F0 - slot 7, GetAgileTypeId.  `retn 0`.  Literal FourCC
//  0x7461736B decodes byte-for-byte (MSB first) as "task".
//----------------------------------------------------------------------------
unsigned int CTask::GetAgileTypeId() const
{
    return 0x7461736B;   // 'task'
}
