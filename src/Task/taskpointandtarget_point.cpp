//============================================================================
//  CTaskPointAndTarget's own point/target-taking vtable overrides -
//  IsPointValid (override of CTaskPoint's own slot 32), TakePoint (override
//  of CTaskPoint's own slot 33) and TakeTarget (brand-new slot 34) - all
//  clustered at 0x6F2884xx-0x6F2885xx, right next to each other.
//
//  Naming for the new slot 34: its own body is TakePoint's exact mirror
//  image - TakePoint here processes m_target first (via sub_6F47B780) then
//  m_moveRequest (via CopyInto, sub_6F47B7A0); slot 34 processes
//  m_moveRequest first (via sub_6F47B780) then m_target (via CopyInto).
//  TakePoint's own name (inherited from CTaskPoint) names the *point*
//  (m_moveRequest) as the field CopyInto actually copies out; by the same
//  reading, slot 34 is the one where CopyInto runs on m_target instead -
//  "TakeTarget", the target-side counterpart the class's own name (Point
//  *and* Target) implies.  sub_6F47B780 itself is not reconstructed (out of
//  scope, same boundary CopyInto/SetFlag/Unregister already draw
//  elsewhere in this family) - its own read (LookupHandle a handle pair
//  out of `*dest`, then call sub_6F47B5B0(this, result)) is structurally
//  different from CopyInto's, so it is declared under its own name here
//  rather than reused as a second CopyInto overload.
//============================================================================
#include "ctaskpointandtarget.h"
#include "../Agent/refcnt.h"
#include "../Game/game.h"

//  Minimal, this-file-only view of the second not-reconstructed
//  MovementRequest entry point this class's own TakePoint/TakeTarget call -
//  0x6F47B780, distinct from CopyInto (0x6F47B7A0, taskpoint.h).  Declared
//  as its own local class (not added to taskpoint.h's own MovementRequest)
//  so this TU's extra member never has to agree with what any other TU
//  declares for the same class name - same reasoning tasktarget_target.cpp
//  already gives for its own local MovementRequest view.
class MovementRequestSecondCopy
{
public:
    void MergeFromDest(void* dest);   // 0x6F47B780, not reconstructed
};

//----------------------------------------------------------------------------
//  0x6F2884D0 - slot 32, IsPointValid (override of CTaskPoint's own).
//  `retn 0`, no stack arguments.  Valid if *either* m_moveRequest or
//  m_target still resolves through LookupHandle - unlike CTaskPoint's own,
//  which only checks m_moveRequest.
//----------------------------------------------------------------------------
int CTaskPointAndTarget::IsPointValid() const
{
    //  A shared-exit goto, not two independent `return 1;` statements -
    //  the dump's two success paths (m_moveRequest resolves, or failing
    //  that m_target resolves) jump to the *same* `mov eax,1 / retn` tail
    //  (docs/msvc-vc8-idioms.md, "Shared exits need goto") - two textually
    //  identical plain returns compile to two different epilogues here
    //  instead.  Mirrors CTaskPoint::IsPointValid's own goto, just for the
    //  success side instead of the failure side.
    MovementRequest* move = m_moveRequest;
    if (move != 0 &&
        LookupHandle(move->m_handle, (int)move->m_typeTag) != 0)
        goto valid;

    {
        MovementRequest* target = m_target;
        if (target != 0 &&
            LookupHandle(target->m_handle, (int)target->m_typeTag) != 0)
            goto valid;
    }

    return 0;

valid:
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F288510 - slot 33, TakePoint (override of CTaskPoint's own).  `retn
//  4`.  Unlike CTaskPoint::TakePoint (which calls CopyInto unconditionally,
//  with no null/validity check), both fields here are guarded by
//  LookupHandle first - m_target processed first (via
//  MergeFromDest/0x6F47B780), then m_moveRequest (via CopyInto/
//  0x6F47B7A0) - each released and cleared right after its own call,
//  whether or not the guard let the call through.
//----------------------------------------------------------------------------
void CTaskPointAndTarget::TakePoint(void* dest)
{
    void* buf = reinterpret_cast<char*>(dest) + 0x164;

    {
        MovementRequest* target = m_target;
        if (target != 0 &&
            LookupHandle(target->m_handle, (int)target->m_typeTag) != 0)
        {
            reinterpret_cast<MovementRequestSecondCopy*>(target)->MergeFromDest(buf);
        }
    }
    {
        TRefCnt* targetRef = reinterpret_cast<TRefCnt*>(m_target);
        if (targetRef != 0 && --targetRef->m_refcount == 0)
            targetRef->ReleaseSelf();
        m_target = 0;
    }

    {
        MovementRequest* move = m_moveRequest;
        if (move != 0 &&
            LookupHandle(move->m_handle, (int)move->m_typeTag) != 0)
        {
            move->CopyInto(buf);
        }
    }
    {
        TRefCnt* moveRef = reinterpret_cast<TRefCnt*>(m_moveRequest);
        if (moveRef != 0 && --moveRef->m_refcount == 0)
            moveRef->ReleaseSelf();
        m_moveRequest = 0;
    }
}

//----------------------------------------------------------------------------
//  0x6F2885A0 - slot 34, TakeTarget (NEW, not in CTaskPoint).  `retn 4`.
//  TakePoint's exact mirror: m_moveRequest processed first (via
//  MergeFromDest/0x6F47B780), then m_target (via CopyInto/0x6F47B7A0) -
//  each released and cleared right after its own call.
//----------------------------------------------------------------------------
void CTaskPointAndTarget::TakeTarget(void* dest)
{
    void* buf = reinterpret_cast<char*>(dest) + 0x164;

    MovementRequest* move = m_moveRequest;
    if (move != 0 &&
        LookupHandle(move->m_handle, (int)move->m_typeTag) != 0)
    {
        reinterpret_cast<MovementRequestSecondCopy*>(move)->MergeFromDest(buf);
    }
    TRefCnt* moveRef = reinterpret_cast<TRefCnt*>(m_moveRequest);
    if (moveRef != 0 && --moveRef->m_refcount == 0)
        moveRef->ReleaseSelf();
    m_moveRequest = 0;

    MovementRequest* target = m_target;
    if (target != 0 &&
        LookupHandle(target->m_handle, (int)target->m_typeTag) != 0)
    {
        target->CopyInto(buf);
    }
    TRefCnt* targetRef = reinterpret_cast<TRefCnt*>(m_target);
    if (targetRef != 0 && --targetRef->m_refcount == 0)
        targetRef->ReleaseSelf();
    m_target = 0;
}
