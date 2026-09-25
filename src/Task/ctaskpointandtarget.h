//============================================================================
//  CTaskPointAndTarget: derives straight from CTaskPoint (taskpoint.h).
//  RTTI-confirmed chain (agent_worktrees/classes/0x6F9306CC.json's own
//  `hierarchy`):
//
//    CTaskPointAndTarget -> CTaskPoint -> CTask -> CUnitRefList
//                         -> CAgentWar3 -> CAgent -> CObserver -> TRefCnt
//
//  Vtable `??_7CTaskPointAndTarget@@6B@` @ 0x6F9306CC (35 slots - one more
//  than CTaskPoint's own 34) overrides 11 of CTaskPoint's 34 slots PLUS one
//  brand-new slot (34) - confirmed by diffing both classes' own
//  `vtable_members` directly (this wave's plan had the count right: unlike
//  CTaskPoint2, this class *does* re-override Save/Load AND both of
//  CTaskPoint's own new slots, IsPointValid/TakePoint):
//
//    slot  1 (0x6F28A9F0)  DeleteSelf         - taskpointandtarget_deleteself.cpp
//                          (own TU, NOT ../Agent/agentwar3_dtor.cpp - like
//                          CTaskPoint's/CTaskTarget's own slot 1, this one
//                          makes a real out-of-line `call` into
//                          ~CTaskPointAndTarget rather than inlining
//                          ~CAgentWar3, because ~CTaskPointAndTarget itself
//                          has real work to do - see below)
//    slot  7 (0x6F271B70)  GetAgileTypeId     - taskpointandtarget_identity.cpp
//    slot 10 (0x6F2883C0)  GetRecordVersion   - taskpointandtarget_save.cpp
//    slot 14 (0x6F288650)  Save               - taskpointandtarget_save.cpp
//    slot 15 (0x6F2886B0)  Load               - taskpointandtarget_save.cpp
//    slot 17 (0x6F288410)  DumpState          - taskpointandtarget_save.cpp
//    slot 18 (0x6F2883D0)  SaveFlags          - taskpointandtarget_save.cpp
//    slot 19 (0x6F288630)  LoadFlags          - taskpointandtarget_save.cpp
//    slot 22 (0x6F271B20)  GetClassName       - taskpointandtarget_identity.cpp
//    slot 32 (0x6F2884D0)  IsPointValid       - taskpointandtarget_point.cpp
//    slot 33 (0x6F288510)  TakePoint          - taskpointandtarget_point.cpp
//    slot 34 (0x6F2885A0)  TakeTarget         - taskpointandtarget_point.cpp (NEW)
//
//  Two new fields beyond CTaskPoint's own +0x50 end, both established
//  directly from Save/SaveFlags/LoadFlags/DumpState's own bodies
//  (taskpointandtarget_save.cpp) and from ConstructCTaskPointAndTarget's
//  own SEH-framed body (dataallocator.h):
//
//    +0x50/+0x54/+0x58  a second SHandleTriple (Agent/handletriple.h) -
//           same {handle, typeTag, extra} shape as CTask's own
//           +0x24/+0x28/+0x2C triple and CTaskTarget's own +0x34/+0x38/+0x3C
//           triple, one instance further down this class's own layout;
//           SaveFlags/LoadFlags round-trip all three exactly the way
//           task.h's own m_field24/28/2C triple is round-tripped (two plain
//           WriteDword/ReadDword-family calls plus one WriteDwordAlt2/
//           ReadHandleTriple-family call), and DumpState dumps only the
//           +0x50/+0x54 pair through SRefMiniValue::DumpTo (0x6F478550),
//           skipping +0x58 - the same "extra field not dumped" shape
//           task.h's own triple already documents.  Not saved/loaded
//           together with m_target below (Save/Load's own handle-pair
//           belongs to m_target, not this triple) - same split
//           tasktarget.h's own m_field34/38/3C draws against m_target
//           there.  Kept the same unexplained-scratch-state naming
//           (m_field50/54/58) tasktarget.h's own m_field34/38/3C uses,
//           for the same reason: nothing in this call tree ties it to a
//           more specific meaning.
//    +0x5C  m_target - a `MovementRequest*` (RTTI-confirmed:
//           `MovementRequest: CPresenceRef, TRefCnt;`,
//           docs/targets/class_informer_dump.tsv) - a second refcounted
//           handle alongside CTaskPoint's own m_moveRequest (+0x4C),
//           analogous to CTaskTarget's own m_target - which is what gives
//           this class its name (it carries both a point, inherited, and a
//           target, its own).  Zeroed at construction; released (its own
//           TRefCnt test/decrement/dispatch-slot-0 shape) and re-zeroed by
//           ~CTaskPointAndTarget and by slots 33/34 alike.  MovementRequest
//           itself is not reconstructed here (out of this session's scope,
//           same boundary CTaskPoint's own m_moveRequest already draws) -
//           nothing in this call tree needs more than its CPresenceRef
//           base, whose `m_handle`/`m_typeTag` pair (Agent/presenceref.h)
//           is exactly what Save/slot 32/slot 33/slot 34 read.
//
//  No explicit constructor is declared, for the same reason task.h gives for
//  CTask: the only reconstructable construction path is entirely inside the
//  SEH-framed thunk (ConstructCTaskPointAndTarget, dataallocator.h/
//  taskpointandtargetinstancegenerator.cpp) - there is no separate,
//  out-of-line "CTaskPointAndTarget::CTaskPointAndTarget" body for a
//  hand-written constructor here to stand in for.
//
//  A real, out-of-line destructor *is* declared (like CTaskPoint/
//  CTaskTarget) - m_target is a refcounted pointer that needs a real
//  release on teardown (on top of the inherited m_moveRequest release
//  CTaskPoint::~CTaskPoint already performs), which is enough to stop the
//  compiler from proving ~CTaskPointAndTarget can't throw, so the shipped
//  body (0x6F288460) opens a real `__except_handler4`-shaped SEH frame this
//  repo's fixed `/GS- /EHs-c-` toolchain cannot emit - kept a `THUNK`, same
//  reasoning as ConstructCTaskPointAndTarget.  Confirmed directly: the
//  shipped body releases only m_target (its own +0x5C) itself, then makes a
//  real out-of-line `call sub_6F280250` into CTaskPoint::~CTaskPoint (a
//  THUNK itself, taskpoint_deleteself.cpp) to release m_moveRequest and
//  tail into ~CAgentWar3 - not two independent releases duplicated here.
//  See taskpointandtarget_deleteself.cpp.
//============================================================================
#ifndef CTASKPOINTANDTARGET_H
#define CTASKPOINTANDTARGET_H

#include "taskpoint.h"

class CTaskPointAndTarget : public CTaskPoint
{
public:
    //  0x6F288460 - see taskpointandtarget_deleteself.cpp.  THUNK: real
    //  __except_handler4-shaped SEH frame, unreproducible at this
    //  toolchain's fixed /GS- /EHs-c-.
    ~CTaskPointAndTarget();

    //  slot 1 / +0x04 (0x6F28A9F0)
    virtual CTaskPointAndTarget* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F271B70)
    virtual unsigned int GetAgileTypeId() const;
    //  slot 10 / +0x28 (0x6F2883C0)
    virtual int GetRecordVersion(int context);
    //  slot 14 / +0x38 (0x6F288650)
    virtual void Save(class CDataStoreScratch* store);
    //  slot 15 / +0x3C (0x6F2886B0)
    virtual void Load(class CDataStore* store);
    //  slot 17 / +0x44 (0x6F288410)
    virtual void DumpState(void* sink);
    //  slot 18 / +0x48 (0x6F2883D0)
    virtual void SaveFlags(CDataStoreScratch* store);
    //  slot 19 / +0x4C (0x6F288630)
    virtual void LoadFlags(CDataStore* store);
    //  slot 22 / +0x58 (0x6F271B20)
    virtual const char* GetClassName();

    //  slot 32 / +0x80 (0x6F2884D0) - override of CTaskPoint's own.  Unlike
    //  CTaskPoint::IsPointValid (m_moveRequest alone), this one is valid if
    //  *either* m_moveRequest or m_target still resolves through
    //  LookupHandle.  See taskpointandtarget_point.cpp.
    virtual int IsPointValid() const;
    //  slot 33 / +0x84 (0x6F288510) - override of CTaskPoint's own.  Takes
    //  m_target first (guarded by LookupHandle, unlike CTaskPoint's own
    //  unconditional CopyInto), then m_moveRequest (also guarded here,
    //  unlike CTaskPoint's own).  See taskpointandtarget_point.cpp.
    virtual void TakePoint(void* dest);
    //  slot 34 / +0x88 (0x6F2885A0) - NEW, not in CTaskPoint.  The mirror
    //  image of TakePoint: takes m_moveRequest first, then m_target -
    //  "TakeTarget", the target-side counterpart TakePoint's own name
    //  implies.  See taskpointandtarget_point.cpp.
    virtual void TakeTarget(void* dest);

    unsigned int m_field50;   // +0x50 - SHandleTriple.m_handle,  -1 unset
    unsigned int m_field54;   // +0x54 - SHandleTriple.m_typeTag, -1 unset
    unsigned int m_field58;   // +0x58 - SHandleTriple.m_extra,    0 unset
    MovementRequest* m_target;  // +0x5C - zeroed at construction
};

#endif
