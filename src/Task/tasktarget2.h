//============================================================================
//  CTaskTarget2: derives straight from CTaskTarget (tasktarget.h).
//  RTTI-confirmed chain (agent_worktrees/classes/0x6F9307E4.json's own
//  `hierarchy`):
//
//    CTaskTarget2 -> CTaskTarget -> CTask -> CUnitRefList -> CAgentWar3
//                 -> CAgent -> CObserver -> TRefCnt
//
//  Vtable `??_7CTaskTarget2@@6B@` @ 0x6F9307E4 (33 slots - same length as
//  CTaskTarget's own 0x6F93075C, no new slot of its own) overrides exactly
//  7 of CTaskTarget's own slots - confirmed by diffing both classes' own
//  `vtable_members` directly rather than trusting the plan handed to this
//  session (which had flagged slots 1,7,10,17,18,19,22 and explicitly *not*
//  14/15 - Save/Load - or 32 - ClearTarget - both verified unchanged here):
//
//    slot  1 (0x6F28AA50)  DeleteSelf         - tasktarget2_dtor.cpp
//                          (own TU, like CTaskTarget's own slot 1 - makes a
//                          real out-of-line `call` into CTaskTarget's own
//                          ~CTaskTarget() (0x6F2802C0, tasktarget.h) rather
//                          than inlining it, because CTaskTarget2 adds no
//                          field of its own that needs destroying beyond
//                          what ~CTaskTarget already tears down - the
//                          compiler-generated ~CTaskTarget2() collapses down
//                          to a plain tail call into its immediate base's
//                          own real, separately-addressed destructor, one
//                          level up the chain from how DeleteSelf's own
//                          header comment describes CItem's/CTaskTarget's)
//    slot  7 (0x6F271C70)  GetAgileTypeId     - tasktarget2_identity.cpp
//    slot 10 (0x6F2889A0)  GetRecordVersion   - tasktarget2_save.cpp
//    slot 17 (0x6F2889F0)  DumpState          - tasktarget2_save.cpp
//    slot 18 (0x6F2889B0)  SaveFlags          - tasktarget2_save.cpp
//    slot 19 (0x6F288A50)  LoadFlags          - tasktarget2_save.cpp
//    slot 22 (0x6F271C20)  GetClassName       - tasktarget2_identity.cpp
//
//  Slots 14/15 (Save/Load) and 32 (ClearTarget) are byte-identical to
//  CTaskTarget's own (0x6F2888C0/0x6F288920/0x6F288730) - inherited
//  unchanged, **not** re-overridden here.
//
//  One new field beyond CTaskTarget's own +0x48 end, established directly
//  from DumpState/SaveFlags/LoadFlags's own bodies (tasktarget2_save.cpp),
//  all three of which touch it as a third SHandleTriple after CTask's own
//  (+0x24/28/2C) and CTaskTarget's own (+0x34/38/3C):
//
//    +0x4C/+0x50/+0x54  a third SHandleTriple (Agent/handletriple.h) - same
//           {handle, typeTag, extra} shape as the two above it; inits to
//           (-1, -1, 0), the same SHandleTriple default the other two
//           document.  SaveFlags/LoadFlags/DumpState round-trip it exactly
//           the way tasktarget.h's own second triple is round-tripped -
//           WriteDword/WriteDword/WriteDwordAlt2 for the three dwords,
//           SRefMiniValue::DumpTo for the {handle,typeTag} pair (m_extra
//           not separately dumped, same as the two triples above it).
//
//  No explicit constructor is declared, for the same reason tasktarget.h
//  gives for CTaskTarget: the only reconstructable construction path is
//  entirely inside the SEH-framed thunk (ConstructCTaskTarget2,
//  tasktarget2instancegenerator.cpp) - there is no separate, out-of-line
//  "CTaskTarget2::CTaskTarget2" body for a hand-written constructor here to
//  stand in for.
//
//  No explicit destructor is declared either, unlike CTaskTarget's own: this
//  class adds only a plain SHandleTriple (no refcounted pointer, no
//  vtabled sub-object) beyond CTaskTarget's own end, so the
//  compiler-generated ~CTaskTarget2() has nothing of its own to release and
//  /O2 collapses it down to a plain tail call straight into ~CTaskTarget()
//  (0x6F2802C0) - see slot 1's own header comment above and
//  tasktarget2_dtor.cpp.
//============================================================================
#ifndef TASKTARGET2_H
#define TASKTARGET2_H

#include "tasktarget.h"

class CTaskTarget2 : public CTaskTarget
{
public:
    //  slot 1 / +0x04 (0x6F28AA50)
    virtual CTaskTarget2* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F271C70)
    virtual unsigned int GetAgileTypeId() const;
    //  slot 10 / +0x28 (0x6F2889A0)
    virtual int GetRecordVersion(int context);
    //  slot 17 / +0x44 (0x6F2889F0)
    virtual void DumpState(void* sink);
    //  slot 18 / +0x48 (0x6F2889B0)
    virtual void SaveFlags(CDataStoreScratch* store);
    //  slot 19 / +0x4C (0x6F288A50)
    virtual void LoadFlags(CDataStore* store);
    //  slot 22 / +0x58 (0x6F271C20)
    virtual const char* GetClassName();

    unsigned int m_field4C;   // +0x4C - SHandleTriple.m_handle,  -1 unset
    unsigned int m_field50;   // +0x50 - SHandleTriple.m_typeTag, -1 unset
    unsigned int m_field54;   // +0x54 - SHandleTriple.m_extra,    0 unset
};

#endif
