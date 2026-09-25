//============================================================================
//  0x6F4737D0 - CPathTrace::AddOriginDelta, in its own translation unit
//  (0x6F47xxxx, apart from CPathTrace.cpp's 0x6F38xxxx/0x6F49xxxx pair) so
//  the calls into GetTravelDistance/PointAtDistance stay real calls.
//============================================================================
#include "game.h"

void CPathTrace::AddOriginDelta(const CGridVec2* originDelta, int flag)
{
    //  A reference to the returned temporary, not a copy: the shipped code
    //  hands GetTravelDistance()'s own result pointer straight on.
    const CFloat& distance = GetTravelDistance();
    CGridVec2 point = PointAtDistance(distance);

    CFloat deltaY = originDelta->m_y - point.m_y;
    CFloat deltaX = originDelta->m_x - point.m_x;

    CGridVec2 combined(deltaX, deltaY);
    ApplyDelta(&combined, flag);
}

//----------------------------------------------------------------------------
//  0x6F497440
//----------------------------------------------------------------------------
void CPathTrace::RefreshSyncSample()
{
    g_pTimeSync->GetChannelSample(this, &m_syncDistance, &m_syncTime);
}

//----------------------------------------------------------------------------
//  0x6F38B140 - CBhPoProjectile::NotifyVelocityDelta's own caller
//  (Missile/bhpoprojectile_notify.cpp). Own TU for the same reason
//  AddOriginDelta above is: GetTravelDistance/PointAtDistance are both
//  defined in CPathTrace.cpp, and `/Ob2` folds a same-TU one-line caller
//  straight into them (this session tried it there first - GetTravelDistance
//  disappeared entirely, replaced by an inlined CTimeSync::Extrapolate
//  call) - moved here so both calls stay real, matching this file's own
//  file header.
//----------------------------------------------------------------------------
CGridVec2 CPathTrace::GetCurrentPosition() const
{
    return PointAtDistance(GetTravelDistance());
}
