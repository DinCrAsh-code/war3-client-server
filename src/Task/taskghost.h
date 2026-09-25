//============================================================================
//  CTaskGhost: derives straight from CTaskOrderParam (taskorderparam.h) -
//  Wave 4, the final class in the CTaskXxx family (task.h's own list).
//  RTTI-confirmed chain (agent_worktrees/classes/0x6F93086C.json's own
//  `hierarchy`):
//
//    CTaskGhost -> CTaskOrderParam -> CTaskParam -> CTask -> CUnitRefList
//               -> CAgentWar3 -> CAgent -> CObserver -> TRefCnt
//
//  Vtable `??_7CTaskGhost@@6B@` @ 0x6F93086C (32 slots, same length as
//  CTaskOrderParam's own 0x6F930524) overrides exactly 7 of
//  CTaskOrderParam's 32 slots - confirmed by diffing both classes' own
//  `vtable_members` directly rather than trusting the plan's own claim
//  (it was right, same double-check discipline every earlier wave in this
//  file used):
//
//    slot  1 (0x6F28AA80)  DeleteSelf         - ../Agent/agentwar3_dtor.cpp
//                          (not a Task/-local file - same reason as every
//                          sibling's own slot 1: this shipped body stamps
//                          ??_7CAgentWar3@@6B@ and tail-calls ~CAgentWar3
//                          inlined, so it has to sit next to
//                          ~CAgentWar3()'s own definition to inline)
//    slot  7 (0x6F271CF0)  GetAgileTypeId     - taskghost_identity.cpp
//    slot 10 (0x6F288AD0)  GetRecordVersion   - taskghost_save.cpp
//    slot 17 (0x6F288B50)  DumpState          - taskghost_save.cpp
//    slot 18 (0x6F288AE0)  SaveFlags          - taskghost_save.cpp
//    slot 19 (0x6F288BD0)  LoadFlags          - taskghost_save.cpp
//    slot 22 (0x6F271CA0)  GetClassName       - taskghost_identity.cpp
//
//  Every other slot is inherited byte-identical from CTaskOrderParam's own
//  bodies (which are themselves CTaskParam's own, unchanged) - confirmed
//  the same way (vtable_members diff, not assumed from the plan) - so none
//  of them are re-overridden here.
//
//  Six new fields beyond CTaskParam's own +0x34/CTaskOrderParam's own
//  (unchanged) end, established directly from CTaskGhost's own real
//  construction path (the SEH-framed "constructor" at 0x6F28F160,
//  CDataAllocator::ConstructCTaskGhost, dataallocator.h - its own inner
//  field-init call, sub_6F288A70, stamps ??_7CTaskGhost@@6B@ and zeroes/
//  stamps exactly these six fields after CTaskParam's own inherited
//  +0x20-+0x34 init) and from DumpState/SaveFlags/LoadFlags's own bodies
//  below, which all agree on the same split:
//
//    +0x38/+0x3C/+0x40/+0x44  four plain dwords, all zeroed at
//           construction, each dumped through SIntMiniValue::DumpTo
//           (same shape as m_field30/m_field34 one class up) and read/
//           written as its own lone WriteDword/ReadDword - never observed
//           together as a triple/handle the way +0x24-+0x2C is.
//    +0x48/+0x4C and +0x50/+0x54  two FloatMini-shaped blocks
//           (Math/floatmini.h - already reconstructed for CItem's own
//           +0x54/+0x5C fields and reused as untyped storage by
//           CTaskRotate/CTaskPoint2, taskrotate.h/taskpoint2.h), each
//           stamped `??_7FloatMini@@6B@` at its own +0x00 and
//           dword_6FAAE470 (g_CFloatZero) copied into its own +0x04 by
//           ConstructCTaskGhost's own inner call, exactly FloatMini's own
//           default constructor, twice over.  DumpState dumps both through
//           FloatMini::DumpTo (0x6F4770E0, non-virtual, same as
//           CTaskRotate's own single field); SaveFlags writes only the
//           raw CFloat bits at +0x4C/+0x54 via WriteCFloat, bypassing each
//           block's own vtable entirely (the shipped call sites never load
//           +0x48/+0x50's vtable pointer at all - same as CTaskRotate's own
//           SaveFlags); LoadFlags reads each raw CFloat back and
//           *publishes* it through the block's own vtable slot 0 (the same
//           notify floatmini.h's own header comment documents), once per
//           block.
//
//  Declared here as six plain fields rather than two actual `FloatMini`
//  members, reinterpret_cast to `FloatMini*` wherever a block's methods are
//  needed - same "raw storage, cast at the point of use" convention
//  taskrotate.h/taskpoint2.h already established, and load-bearing for the
//  same reason those two give: a genuine `FloatMini` member drags in an
//  implicit call to its own (trivial but *virtual*) destructor from the
//  compiler-generated ~CTaskGhost(), which the shipped DeleteSelf
//  disassembly (agentwar3_dtor.cpp) never makes - modelling both blocks as
//  untyped storage is what actually matches, not a shortcut.
//
//  No explicit constructor is declared, for the same reason
//  taskorderparam.h gives for CTaskOrderParam: the only reconstructable
//  construction path is entirely inside the SEH-framed thunk
//  (ConstructCTaskGhost, dataallocator.h/taskghostinstancegenerator.cpp) -
//  there is no separate, out-of-line "CTaskGhost::CTaskGhost" body for a
//  hand-written constructor here to stand in for.
//
//  No explicit destructor is declared either, same reasoning as
//  taskorderparam.h: DeleteSelf (slot 1, ../Agent/agentwar3_dtor.cpp) *is*
//  this family's real teardown path, and CTaskGhost's own six new fields
//  are all plain dwords/untyped-storage FloatMini blocks that need no real
//  destructor work (same "nothing to run a destructor over" shape
//  taskrotate.h's own note gives for its single FloatMini block), so /O2
//  collapses the whole chain down to the same "reset the vtable pointer to
//  CAgentWar3's own and tail-call into CAgent::~CAgent()" shape - confirmed
//  directly by DeleteSelf's own stamped vtable pointer, ??_7CAgentWar3@@6B@,
//  not ??_7CTaskGhost@@6B@.
//============================================================================
#ifndef TASKGHOST_H
#define TASKGHOST_H

#include "taskorderparam.h"
#include "../Math/CFloat.h"

class CTaskGhost : public CTaskOrderParam
{
public:
    //  slot 1 / +0x04 (0x6F28AA80)
    virtual CTaskGhost* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F271CF0)
    virtual unsigned int GetAgileTypeId() const;
    //  slot 10 / +0x28 (0x6F288AD0)
    virtual int GetRecordVersion(int context);
    //  slot 17 / +0x44 (0x6F288B50)
    virtual void DumpState(void* sink);
    //  slot 18 / +0x48 (0x6F288AE0)
    virtual void SaveFlags(class CDataStoreScratch* store);
    //  slot 19 / +0x4C (0x6F288BD0)
    virtual void LoadFlags(class CDataStore* store);
    //  slot 22 / +0x58 (0x6F271CA0)
    virtual const char* GetClassName();

    //  +0x38-+0x44 - see the class comment above.  Four plain dwords,
    //  zeroed by ConstructCTaskGhost (dataallocator.h).
    unsigned int m_field38;   // +0x38
    unsigned int m_field3C;   // +0x3C
    unsigned int m_field40;   // +0x40
    unsigned int m_field44;   // +0x44

    //  +0x48/+0x4C and +0x50/+0x54 - see the class comment above.  Two
    //  FloatMini-shaped blocks, each stamped/zeroed (m_value = g_CFloatZero)
    //  by ConstructCTaskGhost.  Untyped on purpose - see above.
    void*  m_field48;   // +0x48 - first FloatMini's own vtable ptr
    CFloat m_field4C;   // +0x4C - first FloatMini::m_value
    void*  m_field50;   // +0x50 - second FloatMini's own vtable ptr
    CFloat m_field54;   // +0x54 - second FloatMini::m_value
};

#endif
