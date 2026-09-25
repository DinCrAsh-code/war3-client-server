//============================================================================
//  0x6F282920 - CUnit's vtable slot 13 (+0x34), Deactivate.  See unit.h.
//
//  Tear the unit down without destroying it: drop its motion state, run
//  the base's own Deactivate, tell the object behind its +0x304 handle
//  pair, post the "agent going away" event, then release the three handle
//  pairs and owned references it holds and cancel all three of its
//  timers.
//
//  Two of those releases are *inlined* here and one is not.  The +0x130
//  and +0x13C pairs each get the resolve-and-notify block written out in
//  full; the +0x304 pair gets a real call to 0x6F2E5A70
//  (unit_notifyref304.cpp), which is that same block as a function.  That
//  is why the helper lives in a header and the +0x304 one has a
//  translation unit of its own - same shape, and only the out-of-line copy
//  is allowed to stay a call.
//
//  The last Cancel is a tail jump, which is what `pop esi` before the
//  final call is.
//============================================================================
#include "unit.h"
#include "unithandleref.h"

void CUnit::Deactivate()
{
    ClearMotionState();
    CSelectable::Deactivate();
    NotifyRef304Deactivated();
    PostAgentEvent(this, 0);

    ReleaseHandleRefNotify((SOptionalHandleRef*)&m_ref130);
    ReleaseHandleRefNotify((SOptionalHandleRef*)&m_ref13C);

    m_ref128.Release();
    m_ref12C.Release();
    m_ref224.Release();

    m_timer7C.Cancel();
    m_timer68.Cancel();
    m_timer180.Cancel();
}
