//============================================================================
//  CTaskPoint's own two brand-new vtable slots (32/33 - beyond CTask's own
//  32), both operating on m_moveRequest, clustered at 0x6F2880xx-0x6F2881xx
//  right next to Save/Load/DumpState/SaveFlags/LoadFlags (taskpoint_save.cpp)
//  - kept in their own TU purely to group "the new point-specific interface"
//  separately from "the inherited serialisation family".
//
//  Naming (load-bearing for CTaskPoint2/CTaskPointAndTarget, which inherit
//  or override these same two slot indices - see taskpoint.h):
//    slot 32 - IsPointValid() const - a pure query, does m_moveRequest still
//              resolve through LookupHandle right now.
//    slot 33 - TakePoint(void* dest) - writes this task's point data into
//              *dest (at its own +0x164) via MovementRequest::CopyInto, then
//              unconditionally releases and clears m_moveRequest - a single
//              "hand the point to the caller and forget it" transfer, not a
//              copy: after this call IsPointValid() is false.
//============================================================================
#include "taskpoint.h"
#include "../Game/game.h"

//----------------------------------------------------------------------------
//  0x6F288110 - slot 32, IsPointValid.  `retn 0`, no stack arguments (a
//  plain `const` query on `this` alone).
//----------------------------------------------------------------------------
int CTaskPoint::IsPointValid() const
{
    //  A shared-epilogue goto, not two independent `return 0;` statements -
    //  the dump's null-ref check and its failed-LookupHandle check jump to
    //  the *same* `xor eax,eax / retn` tail (docs/msvc-vc8-idioms.md,
    //  "Shared exits need goto"); two textually identical plain returns
    //  compile to two different shapes here instead.
    MovementRequest* ref = m_moveRequest;
    if (ref == 0)
        goto invalid;
    if (LookupHandle(ref->m_handle, (int)ref->m_typeTag) != 0)
        return 1;

invalid:
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F287FF0 - slot 33, TakePoint.  `retn 4`.  The shipped code calls
//  CopyInto *unconditionally*, with no null check on m_moveRequest first -
//  preserved exactly rather than "fixed" with a guard the real function
//  does not have.  The release that follows uses the same test-decrement-
//  ReleaseSelf shape as every other TRefCnt-chain release in this codebase
//  (Agent/destructable_dtor.cpp's own ReleaseTimerRef58, in particular).
//----------------------------------------------------------------------------
void CTaskPoint::TakePoint(void* dest)
{
    m_moveRequest->CopyInto(reinterpret_cast<char*>(dest) + 0x164);

    TRefCnt* ref = m_moveRequest;
    if (ref != 0 && --ref->m_refcount == 0)
        ref->ReleaseSelf();

    m_moveRequest = 0;
}
