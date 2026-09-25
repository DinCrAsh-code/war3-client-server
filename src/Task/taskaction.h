//============================================================================
//  CTaskAction: derives straight from CTaskParam (taskparam.h).
//  RTTI-confirmed chain (agent_worktrees/classes/0x6F93049C.json's own
//  `hierarchy`):
//
//    CTaskAction -> CTaskParam -> CTask -> CUnitRefList -> CAgentWar3
//                -> CAgent -> CObserver -> TRefCnt
//
//  Vtable `??_7CTaskAction@@6B@` @ 0x6F93049C (32 slots, same length as
//  CTaskParam's own 0x6F93038C) overrides exactly 3 of CTaskParam's 32
//  slots - confirmed by diffing both classes' own `vtable_members` directly
//  (per this wave's own instruction to verify rather than trust the plan's
//  claim - it was right):
//
//    slot  1 (0x6F28A930)  DeleteSelf         - ../Agent/agentwar3_dtor.cpp
//                          (not a Task/-local file - same reason as
//                          CTask's/CTaskParam's own slot 1: this shipped
//                          body stamps ??_7CAgentWar3@@6B@ and tail-calls
//                          ~CAgentWar3 inlined, so it has to sit next to
//                          ~CAgentWar3()'s own definition to inline)
//    slot  7 (0x6F271970)  GetAgileTypeId     - taskaction_identity.cpp
//    slot 22 (0x6F271920)  GetClassName       - taskaction_identity.cpp
//
//  All other slots, including GetRecordVersion/DumpState/SaveFlags/LoadFlags
//  (slots 10/17/18/19) and Save/Load (slots 14/15), are byte-identical to
//  CTaskParam's own inherited bodies - **not** re-overridden here.
//
//  No new field beyond CTaskParam's own +0x34 end: CTaskAction's own
//  construction path (the SEH-framed "constructor" at 0x6F28EDD0,
//  CDataAllocator::ConstructCTaskAction, dataallocator.h) zeroes exactly the
//  same six dwords (+0x20 m_flags, +0x24/+0x28 the SHandleTriple pair,
//  +0x2C the third SHandleTriple dword, +0x30, +0x34) that
//  ConstructCTaskParam already zeroes, then stamps ??_7CTaskAction@@6B@
//  instead of ??_7CTaskParam@@6B@ - no further field is ever touched.
//
//  No explicit constructor is declared, for the same reason taskparam.h
//  gives for CTaskParam: the only reconstructable construction path is
//  entirely inside the SEH-framed thunk (ConstructCTaskAction,
//  dataallocator.h/taskactioninstancegenerator.cpp) - there is no separate,
//  out-of-line "CTaskAction::CTaskAction" body for a hand-written
//  constructor here to stand in for.
//
//  No explicit destructor is declared either, same reasoning as
//  taskparam.h: DeleteSelf (slot 1, ../Agent/agentwar3_dtor.cpp) *is* this
//  family's real teardown path, and CTaskAction adds no field at all, so
//  /O2 collapses the whole chain down to the same "reset the vtable
//  pointer to CAgentWar3's own and tail-call into CAgent::~CAgent()" shape
//  - confirmed directly by DeleteSelf's own stamped vtable pointer,
//  ??_7CAgentWar3@@6B@, not ??_7CTaskAction@@6B@.
//
//  CTaskActionFields::Init (0x6F28E3C0) is already reconstructed (see funcmap.py)
//  and is left untouched by this wave.
//============================================================================
#ifndef TASKACTION_H
#define TASKACTION_H

#include "taskparam.h"

class CTaskAction : public CTaskParam
{
public:
    //  slot 1 / +0x04 (0x6F28A930)
    virtual CTaskAction* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F271970)
    virtual unsigned int GetAgileTypeId() const;
    //  slot 22 / +0x58 (0x6F271920)
    virtual const char* GetClassName();
};

#endif
