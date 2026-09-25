//============================================================================
//  0x6F49B950 - CPathClient::PlaceWaypoint: ask the mover to take one step
//  to `pos`, and bracket the move with its own two notifications.
//
//  `this` is never read.  Every field this touches belongs to g_pPathMover,
//  the trace CPathClient::StageSearch latched for this search; it is a
//  member because the shipped callers load ecx before the call.
//
//  A refused step is reported as 0 and nothing is notified.  A step that
//  goes through is sandwiched between CAgent::PostMessage67 and
//  PostMessage68 - the pair every mutation of a trace's origin runs inside -
//  with AddOriginDelta doing the move itself.
//============================================================================
#include "pathmove.h"
#include "agent.h"
#include "game.h"

int CPathClient::PlaceWaypoint(const CGridVec2* pos)
{
    CGridVec2 step(kCFloatNoInit);
    step.m_x = pos->m_x;
    step.m_y = pos->m_y;

    if (g_pPathMover->TryStep(&step, 0x20, 0x18, 6) == 0)
        return 0;

    //  Cast rather than a base-class relation: the shipped code calls the
    //  two senders with the trace in ecx, so CPathTrace and CAgent are the
    //  same object reached from two directions - but making that a real
    //  inheritance would move every field of both classes and a dozen
    //  finished targets depend on their current offsets.  Same reasoning as
    //  pathastarrun.cpp's CPathSearch cast.
    ((CAgent*)g_pPathMover)->PostMessage67();
    g_pPathMover->AddOriginDelta(&step, 0);
    ((CAgent*)g_pPathMover)->PostMessage68();
    return 1;
}
