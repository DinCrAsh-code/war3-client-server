//============================================================================
//  CTaskRotate: one of the ten CTaskXxx siblings task.h lists as not yet
//  reconstructed (this wave does exactly this one).  RTTI-confirmed chain
//  (agent_worktrees/classes/0x6F930414.json's own `hierarchy`):
//
//    CTaskRotate -> CTask -> CUnitRefList -> CAgentWar3 -> CAgent
//                -> CObserver -> TRefCnt
//
//  Vtable `??_7CTaskRotate@@6B@` @ 0x6F930414 (32 slots, same length as
//  CTask's own 0x6F930304) overrides exactly 7 of CTask's 32 slots -
//  confirmed by diffing both classes' own `vtable_members` directly, the
//  same 1/7/10/17/18/19/22 set the plan handed to this session named (and
//  which CTask's own wave already confirmed for the CUnitRefList->CTask
//  step one level up):
//
//    slot  1 (0x6F28A900)  DeleteSelf         - ../Agent/agentwar3_dtor.cpp
//                          (not a Task/-local file - same reason task.h's
//                          own CTask::DeleteSelf lives there: has to sit
//                          next to ~CAgentWar3()'s own definition to inline)
//    slot  7 (0x6F2718F0)  GetAgileTypeId     - taskrotate_identity.cpp
//    slot 10 (0x6F287E00)  GetRecordVersion   - taskrotate_save.cpp
//    slot 17 (0x6F287E70)  DumpState          - taskrotate_save.cpp
//    slot 18 (0x6F287E10)  SaveFlags          - taskrotate_save.cpp
//    slot 19 (0x6F287EC0)  LoadFlags          - taskrotate_save.cpp
//    slot 22 (0x6F2718A0)  GetClassName       - taskrotate_identity.cpp
//
//  One new field beyond CTask's own +0x30 end: a FloatMini-shaped block
//  (Math/floatmini.h - already reconstructed for CItem's own +0x54/+0x5C
//  fields) occupying +0x34 (its own vtable ptr) through +0x38 (its
//  `m_value`, a CFloat).  Established directly from CTaskRotate's own real
//  construction path (CDataAllocator::ConstructCTaskRotate,
//  taskrotateinstancegenerator.cpp - a THUNK, but its raw disassembly
//  stamps `??_7FloatMini@@6B@` at +0x34 and copies dword_6FAAE470
//  (g_CFloatZero) into +0x38, exactly FloatMini's own default constructor)
//  and from DumpState/SaveFlags/LoadFlags's own bodies below, which all
//  agree on the same split: DumpState and LoadFlags operate on it as a
//  FloatMini (FloatMini::DumpTo, and a publish through its own vtable slot
//  0 - the same notify floatmini.h's own header comment and
//  Unit/unit_load.cpp's `PublishFloatMini` helper already establish), while
//  SaveFlags writes the raw CFloat bits straight out of +0x38 via
//  WriteCFloat, bypassing the block's own vtable entirely (matching the
//  shipped SaveFlags call site, which never loads +0x34's vtable pointer at
//  all).
//
//  Declared here as two plain fields rather than an actual `FloatMini`
//  member, reinterpret_cast to `FloatMini*` wherever its methods are
//  needed - the same "raw storage, cast at the point of use" convention
//  task_save.cpp's own SIntMiniValue/SRefMiniValue fields already use one
//  class up.  It is load-bearing here, not just style: a genuine `FloatMini`
//  member drags in an implicit call to its own (trivial but *virtual*)
//  destructor from the compiler-generated ~CTaskRotate(), and unlike
//  CDestructable::~CDestructable's own m_floatB (agentwar3_dtor.cpp) -
//  eliminated as a dead store there - this one sits directly in front of
//  the real, opaque `call CAgent::~CAgent()` DeleteSelf's own inlined
//  ~CAgentWar3() tail makes, which the optimiser cannot see past (that call
//  receives `this`, so it could in principle read any field, +0x34
//  included) and so cannot drop the store on the strength of "never read
//  again" the way it can for m_floatB, sitting entirely inside one leaf
//  function with no such call downstream of it.  The shipped DeleteSelf
//  disassembly confirms there is no +0x34 store there at all - i.e. the
//  real source never ran a destructor over this field either - so modelling
//  it as untyped storage is not a shortcut, it is what actually matches.
//
//  No explicit constructor or destructor is declared, for the same reason
//  task.h gives for CTask itself: the only reconstructable construction
//  path is the SEH-framed ConstructCTaskRotate thunk, and DeleteSelf (slot
//  1) is this family's real teardown path.
//============================================================================
#ifndef TASKROTATE_H
#define TASKROTATE_H

#include "task.h"
#include "../Math/CFloat.h"

class CTaskRotate : public CTask
{
public:
    //  slot 1 / +0x04 (0x6F28A900)
    virtual CTaskRotate* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F2718F0)
    virtual unsigned int GetAgileTypeId() const;
    //  slot 10 / +0x28 (0x6F287E00)
    virtual int GetRecordVersion(int context);
    //  slot 17 / +0x44 (0x6F287E70)
    virtual void DumpState(void* sink);
    //  slot 18 / +0x48 (0x6F287E10)
    virtual void SaveFlags(class CDataStoreScratch* store);
    //  slot 19 / +0x4C (0x6F287EC0)
    virtual void LoadFlags(class CDataStore* store);
    //  slot 22 / +0x58 (0x6F2718A0)
    virtual const char* GetClassName();

    //  +0x34/+0x38 - see the class comment above.  A FloatMini-shaped block,
    //  stamped/zeroed (m_value = g_CFloatZero) by ConstructCTaskRotate
    //  (taskrotateinstancegenerator.cpp).  Untyped on purpose - see above.
    void*  m_field34;   // +0x34 - FloatMini's own vtable ptr
    CFloat m_field38;   // +0x38 - FloatMini::m_value
};

#endif
