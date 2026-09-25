//============================================================================
//  CTaskAction's two "what am I" vtable overrides - both two-instruction
//  leaves, clustered at 0x6F27192x/0x6F2719xx right next to CTaskAction's
//  own InstanceGenerator singleton getter (0x6F271930, own TU for the
//  reason ctaskactioninstancegenerator.h gives), so kept in their own TU
//  here rather than pulled into that one - same shape as CTaskParam's own
//  taskparam_identity.cpp.
//============================================================================
#include "taskaction.h"

//----------------------------------------------------------------------------
//  0x6F271920 - slot 22, GetClassName.  `retn 0`.
//----------------------------------------------------------------------------
const char* CTaskAction::GetClassName()
{
    return "CTaskAction";
}

//----------------------------------------------------------------------------
//  0x6F271970 - slot 7, GetAgileTypeId.  `retn 0`.  Literal 0x74736B41 -
//  same "tsk<letter>" shape as CTask's own 0x7461736B ("task", MSB-first)
//  and CTaskParam's own 0x74736B50 ("tskP", MSB-first) but with the last
//  byte 0x41 ('A') instead of 'P' - not a clean readable word on its own,
//  same reasoning taskparam_identity.cpp gives for its own value; kept as
//  the literal value the shipped code returns rather than forcing a
//  reading that is not actually there.
//----------------------------------------------------------------------------
unsigned int CTaskAction::GetAgileTypeId() const
{
    return 0x74736B41;
}
