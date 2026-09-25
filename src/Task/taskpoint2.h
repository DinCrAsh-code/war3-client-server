//============================================================================
//  CTaskPoint2: derives straight from CTaskPoint (taskpoint.h).
//  RTTI-confirmed chain (agent_worktrees/classes/0x6F93063C.json's own
//  `hierarchy`):
//
//    CTaskPoint2 -> CTaskPoint -> CTask -> CUnitRefList -> CAgentWar3
//                -> CAgent -> CObserver -> TRefCnt
//
//  Vtable `??_7CTaskPoint2@@6B@` @ 0x6F93063C (34 slots - same count as
//  CTaskPoint's own 0x6F9305AC) overrides exactly 7 of CTaskPoint's 34 slots
//  - confirmed by diffing both classes' own `vtable_members` directly:
//
//    slot  1 (0x6F28A9C0)  DeleteSelf         - taskpoint2_deleteself.cpp
//    slot  7 (0x6F271AF0)  GetAgileTypeId     - taskpoint2_identity.cpp
//    slot 10 (0x6F288280)  GetRecordVersion   - taskpoint2_save.cpp
//    slot 17 (0x6F2882D0)  DumpState          - taskpoint2_save.cpp
//    slot 18 (0x6F288290)  SaveFlags          - taskpoint2_save.cpp
//    slot 19 (0x6F288340)  LoadFlags          - taskpoint2_save.cpp
//    slot 22 (0x6F271AA0)  GetClassName       - taskpoint2_identity.cpp
//
//  Slots 14/15 (Save/Load) and slots 32/33 (IsPointValid/TakePoint) are
//  byte-identical to CTaskPoint's own - **not** re-overridden here, matching
//  the plan handed to this session (which had this one right: NOT 32/33,
//  and does not re-override Save/Load).
//
//  Two new fields beyond CTaskPoint's own +0x50 end, both FloatMini-shaped
//  blocks (Math/floatmini.h) - established directly from
//  ConstructCTaskPoint2's own SEH-framed body (dataallocator.h) stamping
//  ??_7FloatMini@@6B@ at +0x50 and +0x58, and from
//  DumpState/SaveFlags/LoadFlags's own bodies (taskpoint2_save.cpp)
//  reading/writing exactly those two offsets - never a third one, unlike
//  CTaskPoint's own three-FloatMini point.  Kept the plain names
//  m_pointX2/m_pointY2 rather than inventing a third "Z" sibling the call
//  tree never touches.
//
//  Declared as plain fields rather than an actual `FloatMini` member,
//  reinterpret_cast to `FloatMini*` wherever its methods are needed - same
//  reason and same shape taskrotate.h's own m_field34/m_field38 already
//  use, load-bearing here too, not just style: a genuine `FloatMini`
//  member gives CTaskPoint2 a non-trivial member sub-object, so the
//  compiler-generated ~CTaskPoint2() (see below) stops being trivial and
//  emits two real, inline vtable-stamp stores the shipped DeleteSelf never
//  has - confirmed directly by diffing DeleteSelf's own dump (which goes
//  straight from `mov esi, ecx` into `call sub_6F280250`, no field
//  teardown at all) against a first-pass typed-FloatMini build (which
//  inserted exactly that pair of stores ahead of the same call, costing
//  DeleteSelf 3 of its 16 generated instructions).  Untyped fields carry
//  no destructor of their own, so ~CTaskPoint2() stays trivial and this
//  problem does not arise.
//
//  No explicit constructor is declared, for the same reason task.h gives
//  for CTask: the only reconstructable construction path is entirely
//  inside the SEH-framed thunk (ConstructCTaskPoint2,
//  dataallocator.h/taskpoint2instancegenerator.cpp) - there is no
//  separate, out-of-line "CTaskPoint2::CTaskPoint2" body for a hand-written
//  constructor here to stand in for.
//
//  No explicit destructor is declared either, unlike CTaskPoint's own: with
//  both new fields untyped (see above), the compiler-generated
//  ~CTaskPoint2() is trivial and collapses down to a plain tail-call into
//  ~CTaskPoint() (0x6F280250, a THUNK, never inlinable regardless of
//  translation unit) - confirmed directly: DeleteSelf's own real
//  out-of-line call (0x6F28A9C3) targets 0x6F280250, CTaskPoint's own
//  destructor address, not a new, CTaskPoint2-owned one.  See
//  taskpoint2_deleteself.cpp.
//============================================================================
#ifndef TASKPOINT2_H
#define TASKPOINT2_H

#include "taskpoint.h"

class CTaskPoint2 : public CTaskPoint
{
public:
    //  slot 1 / +0x04 (0x6F28A9C0)
    virtual CTaskPoint2* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F271AF0)
    virtual unsigned int GetAgileTypeId() const;
    //  slot 10 / +0x28 (0x6F288280)
    virtual int GetRecordVersion(int context);
    //  slot 17 / +0x44 (0x6F2882D0)
    virtual void DumpState(void* sink);
    //  slot 18 / +0x48 (0x6F288290)
    virtual void SaveFlags(class CDataStoreScratch* store);
    //  slot 19 / +0x4C (0x6F288340)
    virtual void LoadFlags(class CDataStore* store);
    //  slot 22 / +0x58 (0x6F271AA0)
    virtual const char* GetClassName();

    //  +0x50/+0x58 - see the class comment above.  Two FloatMini-shaped
    //  blocks, each vtable ptr then CFloat value, stamped/zeroed by
    //  ConstructCTaskPoint2 (taskpoint2instancegenerator.cpp).  Untyped on
    //  purpose - see above.
    void*  m_pointX2Vtable;   // +0x50 - FloatMini's own vtable ptr
    CFloat m_pointX2;         // +0x54 - FloatMini::m_value
    void*  m_pointY2Vtable;   // +0x58 - FloatMini's own vtable ptr
    CFloat m_pointY2;         // +0x5C - FloatMini::m_value
};

#endif
