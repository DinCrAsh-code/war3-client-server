//============================================================================
//  CTaskPoint: derives straight from CTask (task.h).  RTTI-confirmed chain
//  (agent_worktrees/classes/0x6F9305AC.json's own `hierarchy`):
//
//    CTaskPoint -> CTask -> CUnitRefList -> CAgentWar3 -> CAgent -> CObserver
//               -> TRefCnt
//
//  Vtable `??_7CTaskPoint@@6B@` @ 0x6F9305AC (34 slots - two more than
//  CTask's own 32) overrides the same 9 slots CTask itself overrides on top
//  of CUnitRefList, PLUS Save/Load (14/15, which CTaskParam/CTaskRotate do
//  *not* re-override) PLUS two brand-new slots (32/33) - confirmed by
//  diffing both classes' own `vtable_members` directly rather than trusting
//  the plan handed to this session, which had this one right:
//
//    slot  1 (0x6F28A990)  DeleteSelf         - taskpoint_deleteself.cpp
//                          (own TU, NOT ../Agent/agentwar3_dtor.cpp - unlike
//                          CTask's/CTaskParam's own slot 1, this one makes a
//                          real out-of-line `call` into ~CTaskPoint rather
//                          than inlining ~CAgentWar3, because ~CTaskPoint
//                          itself has real work to do - see below)
//    slot  7 (0x6F271A70)  GetAgileTypeId     - taskpoint_identity.cpp
//    slot 10 (0x6F288030)  GetRecordVersion   - taskpoint_save.cpp
//    slot 14 (0x6F2881D0)  Save               - taskpoint_save.cpp
//    slot 15 (0x6F288230)  Load               - taskpoint_save.cpp
//    slot 17 (0x6F2880C0)  DumpState          - taskpoint_save.cpp
//    slot 18 (0x6F288040)  SaveFlags          - taskpoint_save.cpp
//    slot 19 (0x6F288130)  LoadFlags          - taskpoint_save.cpp
//    slot 22 (0x6F271A20)  GetClassName       - taskpoint_identity.cpp
//    slot 32 (0x6F288110)  IsPointValid       - taskpoint_point.cpp (NEW)
//    slot 33 (0x6F287FF0)  TakePoint          - taskpoint_point.cpp (NEW)
//
//  Four new fields beyond CTask's own +0x34 end, all established directly
//  from Save/Load/SaveFlags/LoadFlags/DumpState's own bodies (see
//  taskpoint_save.cpp for exactly how each is read/written) and from
//  ConstructCTaskPoint's own SEH-framed body (dataallocator.h):
//
//    +0x34/+0x3C/+0x44  m_pointX/m_pointY/m_pointZ - three FloatMini
//           sub-objects (Math/floatmini.h, already reconstructed for
//           CItem/CUnit), 8 bytes apart, dumped with FloatMini::DumpTo
//           (0x6F4770E0, the same address DumpState calls three times) and
//           round-tripped through WriteCFloat/ReadCFloat
//           (Net/cdatastore_writecfloat.cpp / cdatastore_readcfloat.cpp) -
//           a plain 3-float point, which is what gives this class its name.
//    +0x4C  m_moveRequest - a `MovementRequest*` (RTTI-confirmed:
//           `MovementRequest: CPresenceRef, TRefCnt;`,
//           docs/targets/class_informer_dump.tsv) - a refcounted handle to
//           wherever this task's point actually resolves to right now.
//           Zeroed at construction (ConstructCTaskPoint); released (its own
//           slot-0 vtable dispatch, TRefCnt's ReleaseSelf shape) and
//           re-zeroed by ~CTaskPoint and by slot 33 (TakePoint) alike.
//           MovementRequest itself is not reconstructed here (out of this
//           session's scope - nothing in this call tree needs more than its
//           CPresenceRef base, whose `m_handle`/`m_typeTag` pair
//           (Agent/presenceref.h) is exactly what Save/Load/slot 32 read).
//
//  No explicit constructor is declared, for the same reason task.h gives for
//  CTask: the only reconstructable construction path is entirely inside the
//  SEH-framed thunk (ConstructCTaskPoint, dataallocator.h/
//  taskpointinstancegenerator.cpp) - there is no separate, out-of-line
//  "CTaskPoint::CTaskPoint" body for a hand-written constructor here to
//  stand in for.
//
//  A real, out-of-line destructor *is* declared (unlike CTask/CTaskParam/
//  CTaskRotate) - m_moveRequest is a refcounted pointer that needs a real
//  release on teardown, which is enough to stop the compiler from proving
//  ~CTaskPoint can't throw, so the shipped body (0x6F280250) opens a real
//  `__except_handler4`-shaped SEH frame this repo's fixed `/GS- /EHs-c-`
//  toolchain cannot emit - kept a `THUNK`, same reasoning as
//  ConstructCTaskPoint. See taskpoint_deleteself.cpp.
//============================================================================
#ifndef TASKPOINT_H
#define TASKPOINT_H

#include "task.h"
#include "../Math/floatmini.h"
#include "../Agent/presenceref.h"

//  RTTI-confirmed: `MovementRequest: CPresenceRef, TRefCnt;`
//  (docs/targets/class_informer_dump.tsv).  Not fully reconstructed here -
//  only enough to give m_moveRequest's fields (inherited from CPresenceRef:
//  m_handle/m_typeTag) and its one real callee (CopyInto) the right shapes;
//  see taskpoint.h's own class comment above and taskpoint_point.cpp.
class MovementRequest : public CPresenceRef
{
public:
    //  0x6F47B7A0 - not reconstructed this session (out of CTaskPoint's own
    //  scope - a module of MovementRequest's own, not CTask's).  Declared
    //  as a member purely for the calling convention (`this` in ecx, `dest`
    //  on the stack, `retn 4`) - same reasoning dataallocator.h gives for
    //  ConstructCWidget/ConstructCTask and friends.
    void CopyInto(void* dest);
};

class CTaskPoint : public CTask
{
public:
    //  0x6F280250 - see taskpoint_deleteself.cpp.  THUNK: real
    //  __except_handler4-shaped SEH frame, unreproducible at this
    //  toolchain's fixed /GS- /EHs-c-.
    ~CTaskPoint();

    //  slot 1 / +0x04 (0x6F28A990)
    virtual CTaskPoint* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F271A70)
    virtual unsigned int GetAgileTypeId() const;
    //  slot 10 / +0x28 (0x6F288030)
    virtual int GetRecordVersion(int context);
    //  slot 14 / +0x38 (0x6F2881D0)
    virtual void Save(class CDataStoreScratch* store);
    //  slot 15 / +0x3C (0x6F288230)
    virtual void Load(class CDataStore* store);
    //  slot 17 / +0x44 (0x6F2880C0)
    virtual void DumpState(void* sink);
    //  slot 18 / +0x48 (0x6F288040)
    virtual void SaveFlags(CDataStoreScratch* store);
    //  slot 19 / +0x4C (0x6F288130)
    virtual void LoadFlags(CDataStore* store);
    //  slot 22 / +0x58 (0x6F271A20)
    virtual const char* GetClassName();

    //  slot 32 / +0x80 (0x6F288110) - NEW, not in CTask.  A boolean query
    //  (does m_moveRequest still resolve through LookupHandle?) but
    //  declared `int`, not `bool` - the shipped body sets/clears the *full*
    //  `eax` (`mov eax,1` / `xor eax,eax`), not `al` alone, which is what
    //  this toolchain emits for a real `bool` return here.  See
    //  taskpoint_point.cpp for why this name and not something else.
    virtual int IsPointValid() const;
    //  slot 33 / +0x84 (0x6F287FF0) - NEW, not in CTask.  Writes this
    //  task's point data into `*dest` (at its own +0x164), then releases
    //  and clears m_moveRequest - a one-shot "hand the point to the caller
    //  and forget it" transfer.  See taskpoint_point.cpp.
    virtual void TakePoint(void* dest);

    FloatMini      m_pointX;       // +0x34
    FloatMini      m_pointY;       // +0x3C
    FloatMini      m_pointZ;       // +0x44
    MovementRequest* m_moveRequest;  // +0x4C - zeroed at construction
};

#endif
