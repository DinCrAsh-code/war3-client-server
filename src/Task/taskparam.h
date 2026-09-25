//============================================================================
//  CTaskParam: derives straight from CTask (task.h).  RTTI-confirmed chain
//  (agent_worktrees/classes/0x6F93038C.json's own `hierarchy`):
//
//    CTaskParam -> CTask -> CUnitRefList -> CAgentWar3 -> CAgent -> CObserver
//               -> TRefCnt
//
//  Vtable `??_7CTaskParam@@6B@` @ 0x6F93038C (32 slots, same length as
//  CTask's own 0x6F930304) overrides exactly the same 7 slots CTask itself
//  overrides on top of CUnitRefList - confirmed by diffing both classes' own
//  `vtable_members` directly, per this wave's own correction note to double
//  check the plan's 7-slot claim rather than trust it (it was right):
//
//    slot  1 (0x6F28A8D0)  DeleteSelf         - ../Agent/agentwar3_dtor.cpp
//                          (not a Task/-local file - same reason as CTask's
//                          own slot 1: this shipped body stamps
//                          ??_7CAgentWar3@@6B@ and tail-calls ~CAgentWar3
//                          inlined, so it has to sit next to
//                          ~CAgentWar3()'s own definition to inline)
//    slot  7 (0x6F271870)  GetAgileTypeId     - taskparam_identity.cpp
//    slot 10 (0x6F287CD0)  GetRecordVersion   - taskparam_save.cpp
//    slot 17 (0x6F287D30)  DumpState          - taskparam_save.cpp
//    slot 18 (0x6F287CE0)  SaveFlags          - taskparam_save.cpp
//    slot 19 (0x6F287D70)  LoadFlags          - taskparam_save.cpp
//    slot 22 (0x6F271820)  GetClassName       - taskparam_identity.cpp
//
//  Slots 14/15 (Save/Load) are byte-identical to CTask's own inherited
//  bodies (0x6F001A30/0x6F001A70) - **not** re-overridden here, same as
//  CTask's own note about inheriting them unchanged from CAgentWar3.
//
//  One new field beyond CTask's own +0x30 end, established directly from
//  CTaskParam's own real construction path (the SEH-framed "constructor" at
//  0x6F28ECC0, CDataAllocator::ConstructCTaskParam, dataallocator.h) and
//  from LoadFlags/SaveFlags/DumpState's own bodies, which all touch it as a
//  fourth, standalone dword right after CTask's own m_field30 - same
//  "zeroed at construction, read/written as a lone dword, never observed as
//  part of the SHandleTriple above it" shape task.h's own m_field30 already
//  documents:
//    +0x34  m_field34 - zeroed at construction.
//
//  No explicit constructor is declared, for the same reason task.h gives
//  for CTask: the only reconstructable construction path is entirely inside
//  the SEH-framed thunk (ConstructCTaskParam,
//  dataallocator.h/taskparaminstancegenerator.cpp) - there is no separate,
//  out-of-line "CTaskParam::CTaskParam" body for a hand-written constructor
//  here to stand in for.
//
//  No explicit destructor is declared either, same reasoning as task.h:
//  DeleteSelf (slot 1, ../Agent/agentwar3_dtor.cpp) *is* this family's real
//  teardown path, and CTaskParam adds no field that needs anything but a
//  trivial destructor (m_field34 is a plain dword), so /O2 collapses the
//  whole chain down to the same "reset the vtable pointer to CAgentWar3's
//  own and tail-call into CAgent::~CAgent()" shape - confirmed directly by
//  DeleteSelf's own stamped vtable pointer, ??_7CAgentWar3@@6B@, not
//  ??_7CTaskParam@@6B@.
//============================================================================
#ifndef TASKPARAM_H
#define TASKPARAM_H

#include "task.h"

class CTaskParam : public CTask
{
public:
    //  slot 1 / +0x04 (0x6F28A8D0)
    virtual CTaskParam* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F271870)
    virtual unsigned int GetAgileTypeId() const;
    //  slot 10 / +0x28 (0x6F287CD0)
    virtual int GetRecordVersion(int context);
    //  slot 17 / +0x44 (0x6F287D30)
    virtual void DumpState(void* sink);
    //  slot 18 / +0x48 (0x6F287CE0)
    virtual void SaveFlags(class CDataStoreScratch* store);
    //  slot 19 / +0x4C (0x6F287D70)
    virtual void LoadFlags(class CDataStore* store);
    //  slot 22 / +0x58 (0x6F271820)
    virtual const char* GetClassName();

    //  +0x34 - see the class comment above.  Zeroed by ConstructCTaskParam
    //  (dataallocator.h).
    unsigned int m_field34;   // +0x34
};

#endif
