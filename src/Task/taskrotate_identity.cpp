//============================================================================
//  CTaskRotate's two "what am I" vtable overrides - both two-instruction
//  leaves, clustered at 0x6F2718Ax/0x6F2718Fx right next to CTaskRotate's
//  own InstanceGenerator singleton getter (0x6F2718B0, own TU for the same
//  reason ctaskrotateinstancegenerator.h gives), so kept in their own TU
//  here rather than pulled into that one - same shape as CTask's own
//  task_identity.cpp, one class down.
//============================================================================
#include "taskrotate.h"

//----------------------------------------------------------------------------
//  0x6F2718A0 - slot 22, GetClassName.  `retn 0`.
//----------------------------------------------------------------------------
const char* CTaskRotate::GetClassName()
{
    return "CTaskRotate";
}

//----------------------------------------------------------------------------
//  0x6F2718F0 - slot 7, GetAgileTypeId.  `retn 0`.  Literal FourCC
//  0x74736B52 - same {'t','s','k',x} shape as CTask's own 0x7461736B
//  ('task'), with the trailing byte swapped for this subclass's own tag
//  rather than spelling out a legible word (CTask's own comment,
//  task_identity.cpp, calls this out as MSB-first; here that reads
//  't','s','k','R' - not an English word, but the same per-subclass-tag
//  convention InstanceGenerator<CTaskRotate>'s own RTTI type-descriptor
//  string, "AVCTaskRotate", already confirms for this class).
//----------------------------------------------------------------------------
unsigned int CTaskRotate::GetAgileTypeId() const
{
    return 0x74736B52;
}
