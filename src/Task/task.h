//============================================================================
//  CTask: the base of the whole CTaskXxx family (CTaskParam, CTaskRotate,
//  CTaskAction, CTaskOrderParam, CTaskPoint, CTaskPoint2,
//  CTaskPointAndTarget, CTaskTarget, CTaskTarget2, CTaskGhost - none
//  reconstructed here; this wave is CTask's own base only).  RTTI-confirmed
//  chain (agent_worktrees/classes/0x6F930304.json's own `hierarchy`):
//
//    CTask -> CUnitRefList -> CAgentWar3 -> CAgent -> CObserver -> TRefCnt
//
//  Read unitreflist.h/agentwar3.h/agent.h before touching this file - every
//  offset below is anchored to CUnitRefList's own end (+0x2C).
//
//  Vtable `??_7CTask@@6B@` @ 0x6F930304 (32 slots, same length as
//  CUnitRefList's own 0x6F87798C) overrides exactly 7 of CUnitRefList's 32
//  slots - confirmed by diffing both classes' own `vtable_members` directly
//  rather than trusting the plan handed to this session (which had
//  initially listed 9, including 14/15 - Save/Load - before checking):
//
//    slot  1 (0x6F28A8A0)  DeleteSelf         - ../Agent/agentwar3_dtor.cpp
//                          (not a Task/-local file - see that file's own
//                          header comment for why: it has to sit next to
//                          ~CAgentWar3()'s own definition to inline)
//    slot  7 (0x6F2717F0)  GetAgileTypeId     - task_identity.cpp
//    slot 10 (0x6F287BD0)  GetRecordVersion   - task_save.cpp
//    slot 17 (0x6F287C30)  DumpState          - task_save.cpp
//    slot 18 (0x6F287BE0)  SaveFlags          - task_save.cpp
//    slot 19 (0x6F287C60)  LoadFlags          - task_save.cpp
//    slot 22 (0x6F2717A0)  GetClassName       - task_identity.cpp
//
//  Slots 14/15 (Save/Load) are byte-identical to CUnitRefList's own
//  (0x6F001A30/0x6F001A70, CAgentWar3's own bodies, inherited unchanged
//  all the way down) - **not** re-overridden here, despite the plan's own
//  first pass assuming they were.
//
//  Two new fields beyond CUnitRefList's own +0x2C end, both established
//  directly from CTask's own real construction path (the SEH-framed
//  "constructor" at 0x6F28EC40, CDataAllocator::ConstructCTask,
//  dataallocator.h) and from LoadFlags/SaveFlags/DumpState's own bodies
//  reading/writing them:
//    +0x2C  m_field2C - zeroed at construction.  Together with
//           CUnitRefList's own m_field24/m_field28 (unitreflist.h) it forms
//           one contiguous 3-dword SHandleTriple (handletriple.h) -
//           LoadFlags reads all three in one ReadHandleTriple(store,
//           (SHandleTriple*)&m_field24) call, and its own default values
//           (-1, -1, 0) are exactly SHandleTriple's own documented
//           construction default (position.h).  Declared here rather than
//           folded into unitreflist.h because CUnitRefList's own two
//           fields were already established, independently, by an earlier
//           session (CAbility work) - this file only adds the third dword
//           that completes the triple.
//    +0x30  m_field30 - zeroed at construction; read/written as a lone
//           dword by DumpState/SaveFlags/LoadFlags alongside the
//           SHandleTriple above, but never observed as part of it (its own
//           ReadDword/WriteDword call is separate from the
//           ReadHandleTriple/three-WriteDword-calls that cover +0x24-+0x2C).
//
//  No explicit constructor is declared, for the same reason jassfunc.h
//  gives for CJassFunc: the only reconstructable construction path is
//  entirely inside the SEH-framed thunk (ConstructCTask,
//  dataallocator.h/taskinstancegenerator.cpp) - there is no separate,
//  out-of-line "CTask::CTask" body for a hand-written constructor here to
//  stand in for.
//
//  No explicit destructor is declared either: DeleteSelf (slot 1,
//  ../Agent/agentwar3_dtor.cpp) *is* this family's real teardown path (agent.h's
//  own note - "deliberately not a virtual destructor... DeleteSelf
//  instead"), and its own body relies on the ordinary compiler-generated
//  ~CTask() - CTask and CUnitRefList each have nothing of their own to
//  destroy, so /O2 collapses that whole chain down to "reset the vtable
//  pointer to CAgentWar3's own (the nearest base with a real, out-of-line
//  destructor) and tail-call into CAgent::~CAgent()" - the exact shape
//  agentwar3_dtor.cpp's own header comment already documents for the same
//  reason, one level up the chain.
//============================================================================
#ifndef TASK_H
#define TASK_H

#include "../Unit/unitreflist.h"

class CTask : public CUnitRefList
{
public:
    //  slot 1 / +0x04 (0x6F28A8A0)
    virtual CTask* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F2717F0)
    virtual unsigned int GetAgileTypeId() const;
    //  slot 10 / +0x28 (0x6F287BD0)
    virtual int GetRecordVersion(int context);
    //  slot 17 / +0x44 (0x6F287C30)
    virtual void DumpState(void* sink);
    //  slot 18 / +0x48 (0x6F287BE0)
    virtual void SaveFlags(class CDataStoreScratch* store);
    //  slot 19 / +0x4C (0x6F287C60)
    virtual void LoadFlags(class CDataStore* store);
    //  slot 22 / +0x58 (0x6F2717A0)
    virtual const char* GetClassName();

    //  +0x2C / +0x30 - see the class comment above.  Both zeroed by
    //  ConstructCTask (dataallocator.h).
    unsigned int m_field2C;   // +0x2C
    unsigned int m_field30;   // +0x30
};

#endif
