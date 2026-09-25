//============================================================================
//  CTaskTarget: derives straight from CTask (task.h).  RTTI-confirmed chain
//  (agent_worktrees/classes/0x6F93075C.json's own `hierarchy`):
//
//    CTaskTarget -> CTask -> CUnitRefList -> CAgentWar3 -> CAgent -> CObserver
//                -> TRefCnt
//
//  Vtable `??_7CTaskTarget@@6B@` @ 0x6F93075C (33 slots - one more than
//  CTask's own 32) overrides the same 9 slots CTask itself overrides on top
//  of CUnitRefList (including Save/Load, 14/15, which CTaskParam/CTaskRotate
//  do *not* re-override - CTaskTarget has real per-instance state that needs
//  its own round-trip, the same reason CTaskPoint re-overrides them) PLUS one
//  brand-new slot (32) - confirmed by diffing both classes' own
//  `vtable_members` directly:
//
//    slot  1 (0x6F28AA20)  DeleteSelf         - tasktarget_dtor.cpp
//                          (own TU, NOT ../Agent/agentwar3_dtor.cpp - like
//                          CTaskPoint's own slot 1, this one makes a real
//                          out-of-line `call` into ~CTaskTarget rather than
//                          inlining ~CAgentWar3, because ~CTaskTarget itself
//                          has real work to do - see below)
//    slot  7 (0x6F271BF0)  GetAgileTypeId     - tasktarget_identity.cpp
//    slot 10 (0x6F288780)  GetRecordVersion   - tasktarget_save.cpp
//    slot 14 (0x6F2888C0)  Save               - tasktarget_save.cpp
//    slot 15 (0x6F288920)  Load               - tasktarget_save.cpp
//    slot 17 (0x6F288810)  DumpState          - tasktarget_save.cpp
//    slot 18 (0x6F288790)  SaveFlags          - tasktarget_save.cpp
//    slot 19 (0x6F288850)  LoadFlags          - tasktarget_save.cpp
//    slot 22 (0x6F271BA0)  GetClassName       - tasktarget_identity.cpp
//    slot 32 (0x6F288730)  ClearTarget        - tasktarget_target.cpp (NEW)
//
//  Five new fields beyond CTask's own +0x30 end, all established directly
//  from ConstructCTaskTarget's own SEH-framed body (0x6F28F050, dataallocator
//  -equivalent thunk, tasktargetinstancegenerator.cpp) and from
//  Save/Load/SaveFlags/LoadFlags/DumpState's own bodies (tasktarget_save.cpp):
//
//    +0x34/+0x38/+0x3C  a second SHandleTriple (Agent/handletriple.h) - same
//           {handle, typeTag, extra} shape as CTask's own +0x24/+0x28/+0x2C
//           triple, just one instance further down this class's own layout;
//           inits to (-1, -1, 0), the same SHandleTriple default CTask's own
//           triple documents.  Not saved/loaded together with m_target below
//           (Save/Load's own handle-pair belongs to m_target, not this
//           triple) - SaveFlags/LoadFlags/DumpState round-trip it exactly
//           the way task.h's own m_field24/28/2C triple is round-tripped.
//    +0x40  m_fieldWeight - a FloatMini sub-object (Math/floatmini.h,
//           already reconstructed for CItem/CUnit): ConstructCTaskTarget
//           stamps `??_7FloatMini@@6B@` at +0x40 and copies g_CFloatZero
//           into +0x44 (FloatMini's own default constructor, unchanged).
//           Dumped (FloatMini::DumpTo, 0x6F4770E0) but never saved - the
//           save-side round-trip SaveFlags/LoadFlags::store->Write/ReadDword
//           at +0x44 is this same FloatMini's own m_value field, so the two
//           halves of the pair do get persisted, just through SaveFlags
//           rather than through a member call on the FloatMini itself.
//    +0x48  m_target - a `MovementRequest*` (RTTI-confirmed:
//           `MovementRequest: CPresenceRef, TRefCnt;`,
//           docs/targets/class_informer_dump.tsv, agent_worktrees/classes/
//           0x6F929FF4.json) - a refcounted handle to whatever this task is
//           currently directed at, which is what gives this class its name.
//           Zeroed at construction; released (its own slot-0 vtable
//           dispatch, TRefCnt's ReleaseSelf shape) and re-zeroed by
//           ~CTaskTarget and by slot 32 (ClearTarget) alike.  MovementRequest
//           itself is not reconstructed here (out of this session's scope,
//           same boundary CTaskPoint's own m_moveRequest already draws) -
//           nothing in this call tree needs more than its CPresenceRef base,
//           whose `m_handle`/`m_typeTag` pair (Agent/presenceref.h) is
//           exactly what Save/slot 32 read.
//
//  No explicit constructor is declared, for the same reason task.h gives for
//  CTask: the only reconstructable construction path is entirely inside the
//  SEH-framed thunk (ConstructCTaskTarget, tasktargetinstancegenerator.cpp) -
//  there is no separate, out-of-line "CTaskTarget::CTaskTarget" body for a
//  hand-written constructor here to stand in for.
//
//  A real, out-of-line destructor *is* declared (unlike CTask/CTaskParam/
//  CTaskRotate, but like CTaskPoint) - m_target is a refcounted pointer that
//  needs a real release on teardown, which is enough to stop the compiler
//  from proving ~CTaskTarget can't throw, so the shipped body (0x6F2802C0)
//  opens a real `__except_handler4`-shaped SEH frame this repo's fixed
//  `/GS- /EHs-c-` toolchain cannot emit - kept a `THUNK`, same reasoning as
//  ConstructCTaskTarget and CTaskPoint's own ~CTaskPoint. See
//  tasktarget_dtor.cpp.
//============================================================================
#ifndef TASKTARGET_H
#define TASKTARGET_H

#include "task.h"
#include "../Math/floatmini.h"

class MovementRequest;
class CUnit;

class CTaskTarget : public CTask
{
public:
    //  0x6F2802C0 - see tasktarget_dtor.cpp.  THUNK: real
    //  __except_handler4-shaped SEH frame, unreproducible at this
    //  toolchain's fixed /GS- /EHs-c-.
    ~CTaskTarget();

    //  slot 1 / +0x04 (0x6F28AA20)
    virtual CTaskTarget* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F271BF0)
    virtual unsigned int GetAgileTypeId() const;
    //  slot 10 / +0x28 (0x6F288780)
    virtual int GetRecordVersion(int context);
    //  slot 14 / +0x38 (0x6F2888C0)
    virtual void Save(class CDataStoreScratch* store);
    //  slot 15 / +0x3C (0x6F288920)
    virtual void Load(class CDataStore* store);
    //  slot 17 / +0x44 (0x6F288810)
    virtual void DumpState(void* sink);
    //  slot 18 / +0x48 (0x6F288790)
    virtual void SaveFlags(CDataStoreScratch* store);
    //  slot 19 / +0x4C (0x6F288850)
    virtual void LoadFlags(CDataStore* store);
    //  slot 22 / +0x58 (0x6F271BA0)
    virtual const char* GetClassName();

    //  slot 32 / +0x80 (0x6F288730) - NEW, not in CTask.  Releases this
    //  task's own m_target reference: if `notify` is set, marks a flag on
    //  whatever m_target currently resolves to (through LookupHandle);
    //  always unregisters this task from `unit`'s own SUnitMovement
    //  sub-object (Unit/unit.h, +0x164); then releases and clears m_target.
    //  Named for what it structurally does - a release, not a query or a
    //  transfer (CTaskPoint's own new slots are named IsPointValid/
    //  TakePoint for exactly those two different shapes; this one is
    //  neither) - see tasktarget_target.cpp for the full read and for why
    //  its two callees are left as THUNKs.
    virtual void ClearTarget(CUnit* unit, int notify);

    unsigned int m_field34;      // +0x34 - SHandleTriple.m_handle,  -1 unset
    unsigned int m_field38;      // +0x38 - SHandleTriple.m_typeTag, -1 unset
    unsigned int m_field3C;      // +0x3C - SHandleTriple.m_extra,    0 unset
    FloatMini    m_fieldWeight;  // +0x40 - vtable at +0x40, m_value at +0x44
                                  //   (g_CFloatZero at construction);
                                  //   persisted through SaveFlags/LoadFlags'
                                  //   own +0x44 dword, not through a member
                                  //   call on this object
    MovementRequest* m_target;   // +0x48 - zeroed at construction
};

#endif
