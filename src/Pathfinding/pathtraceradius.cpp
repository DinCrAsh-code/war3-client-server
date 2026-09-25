//============================================================================
//  0x6F4A7350 - CPathTrace::SetRadius: change the radius both grid
//  registrations grow this trace's position into a rectangle by, and
//  re-register it at once.
//
//  The re-registration goes through RecomputeOrigin with a zero delta, which
//  is the same call ApplyDelta makes for a real move: the origin does not
//  move, but both grid boxes are rebuilt from the new radius.  The zero pair
//  is built out of g_CFloatZero rather than a default-constructed CGridVec2
//  - the shipped code loads that global once and stores it into both
//  components, which is exactly one `mov` per component and not the two the
//  default constructor's own zero would cost.
//============================================================================
#include "game.h"
#include "CFloat.h"

void CPathTrace::SetRadius(const CFloat* radius)
{
    m_radius = *radius;

    CGridVec2 nothing(kCFloatNoInit);
    nothing.m_x = g_CFloatZero;
    nothing.m_y = g_CFloatZero;
    RecomputeOrigin(&nothing);
}
