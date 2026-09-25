//============================================================================
//  0x6F288730 - CTaskTarget's own brand-new vtable slot, 32 - the one slot
//  beyond CTask's own 32 this class adds (see tasktarget.h).  Named
//  ClearTarget: it releases m_target, not a query (CTaskPoint's sibling new
//  slot 32 is IsPointValid, a bool-returning read) and not a transfer
//  (CTaskPoint's slot 33, TakePoint, hands point data to a caller-owned
//  buffer before releasing) - this one takes no output parameter and writes
//  nothing back, it only tears down.
//
//  Two stack arguments (`retn 8`): a `CUnit* unit` whose own +0x164
//  SUnitMovement sub-object (Unit/unit.h) this task's target gets
//  unregistered from, and an `int notify` flag that, when set, marks a bit
//  on whatever m_target currently resolves to before the unregister.
//============================================================================
#include "tasktarget.h"
#include "../Agent/refcnt.h"

//  Minimal, this-file-only view of MovementRequest's own interface - just
//  enough to spell the two calls this slot makes, the same "only touched
//  members get names" rule the field-level reinterpret_casts elsewhere in
//  this class use structurally.  Not the class's own real definition (out
//  of this session's scope, see tasktarget.h's own note); spelled as
//  members rather than free functions purely for the calling convention -
//  a free function cannot be declared explicit `__thiscall` in this
//  toolchain (refcnt.h's own TRefCnt::AddRef note makes the same point
//  about `__fastcall`), and both of these are reached with the target in
//  ecx and one stack argument, `retn 4` - implicit thiscall, which only a
//  member function gets.  Not reconstructed by this wave - the same
//  "genuinely MovementRequest's own machinery" boundary
//  tasktarget_save.cpp's own LoadMovementRequestField (0x6F2871A0) already
//  draws for Load.
class MovementRequest
{
public:
    void SetFlag(int flag);                 // 0x6F47B300, not reconstructed
    void Unregister(void* movementSubObj);   // 0x6F47B7A0, not reconstructed
};

//----------------------------------------------------------------------------
//  0x6F288730 - slot 32, ClearTarget.  `retn 8`.
//----------------------------------------------------------------------------
void CTaskTarget::ClearTarget(CUnit* unit, int notify)
{
    void* movementSubObj = reinterpret_cast<char*>(unit) + 0x164;

    if (notify)
        m_target->SetFlag(1);

    m_target->Unregister(movementSubObj);

    //  Release m_target (TRefCnt's own test/decrement/dispatch-slot-0 shape
    //  - see agentwar3_dtor.cpp's own ReleaseTimerRef58 for the same
    //  pattern spelled out once already) and clear it either way.
    TRefCnt* ref = reinterpret_cast<TRefCnt*>(m_target);
    if (ref != 0 && --ref->m_refcount == 0)
        ref->ReleaseSelf();

    m_target = 0;
}
