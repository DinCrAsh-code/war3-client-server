//============================================================================
//  A move in progress along a straight line.
//============================================================================
#include "game.h"

//----------------------------------------------------------------------------
//  0x6F497460 - how far the move has got by now: the distance recorded at the
//  last synchronisation point, carried forward to the current one.
//----------------------------------------------------------------------------
CFloat CPathTrace::GetTravelDistance() const
{
    return g_pTimeSync->Extrapolate(this, m_syncDistance, m_syncTime);
}

//----------------------------------------------------------------------------
//  0x6F38B0B0 - the point that far along the line, in grid space.
//----------------------------------------------------------------------------
CGridVec2 CPathTrace::PointAtDistance(const CFloat& distance) const
{
    return m_origin + m_direction * distance;
}
