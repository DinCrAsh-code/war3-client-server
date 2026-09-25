//============================================================================
//  CTaskOrderParam: derives straight from CTaskParam (taskparam.h).
//  RTTI-confirmed chain (agent_worktrees/classes/0x6F930524.json's own
//  `hierarchy`):
//
//    CTaskOrderParam -> CTaskParam -> CTask -> CUnitRefList -> CAgentWar3
//                     -> CAgent -> CObserver -> TRefCnt
//
//  Vtable `??_7CTaskOrderParam@@6B@` @ 0x6F930524 (32 slots, same length as
//  CTaskParam's own 0x6F93038C) overrides exactly 3 of CTaskParam's 32
//  slots - confirmed by diffing both classes' own `vtable_members` directly
//  (same double-check discipline this wave's own instructions call for,
//  matching CTaskParam's own 3-slot claim against CTask exactly):
//
//    slot  1 (0x6F28A960)  DeleteSelf         - ../Agent/agentwar3_dtor.cpp
//                          (not a Task/-local file - same reason as
//                          CTask's/CTaskParam's own slot 1: this shipped
//                          body stamps ??_7CAgentWar3@@6B@ and tail-calls
//                          ~CAgentWar3 inlined, so it has to sit next to
//                          ~CAgentWar3()'s own definition to inline)
//    slot  7 (0x6F2719F0)  GetAgileTypeId     - taskorderparam_identity.cpp
//    slot 22 (0x6F2719A0)  GetClassName       - taskorderparam_identity.cpp
//
//  Every other slot, GetRecordVersion/DumpState/SaveFlags/LoadFlags
//  (10/17/18/19) included, is inherited byte-identical from CTaskParam's
//  own bodies - confirmed the same way (vtable_members diff, not assumed
//  from the plan) - so none of them are re-overridden here.
//
//  CTaskOrderParam adds no field beyond CTaskParam's own +0x34 end: its
//  own real construction path (the SEH-framed "constructor" at
//  0x6F28EE50, CDataAllocator::ConstructCTaskOrderParam, dataallocator.h)
//  zeroes/-1's exactly the same six dwords at +0x20-+0x34 that
//  ConstructCTaskParam already does, and stamps only its own vtable
//  (??_7CTaskOrderParam@@6B@) in their place - no seventh field is ever
//  written.
//
//  No explicit constructor is declared, for the same reason taskparam.h
//  gives for CTaskParam: the only reconstructable construction path is
//  entirely inside the SEH-framed thunk (ConstructCTaskOrderParam,
//  dataallocator.h/taskorderparaminstancegenerator.cpp) - there is no
//  separate, out-of-line "CTaskOrderParam::CTaskOrderParam" body for a
//  hand-written constructor here to stand in for.
//
//  No explicit destructor is declared either, same reasoning as
//  taskparam.h: DeleteSelf (slot 1, ../Agent/agentwar3_dtor.cpp) *is* this
//  family's real teardown path, and CTaskOrderParam adds no field of its
//  own that needs anything but a trivial destructor (it adds none at
//  all), so /O2 collapses the whole chain down to the same
//  "reset the vtable pointer to CAgentWar3's own and tail-call into
//  CAgent::~CAgent()" shape - confirmed directly by DeleteSelf's own
//  stamped vtable pointer, ??_7CAgentWar3@@6B@, not
//  ??_7CTaskOrderParam@@6B@.
//============================================================================
#ifndef TASKORDERPARAM_H
#define TASKORDERPARAM_H

#include "taskparam.h"

class CTaskOrderParam : public CTaskParam
{
public:
    //  slot 1 / +0x04 (0x6F28A960)
    virtual CTaskOrderParam* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F2719F0)
    virtual unsigned int GetAgileTypeId() const;
    //  slot 22 / +0x58 (0x6F2719A0)
    virtual const char* GetClassName();
};

#endif
