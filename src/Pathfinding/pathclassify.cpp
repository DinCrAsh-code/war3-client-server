//============================================================================
//  0x6F49C3B0 - CPathClient::Classify: is the cached run still usable from
//  `from` towards `to`? Too far from where it was last seeked resets the
//  whole search; otherwise the two cache cursors decide whether to
//  re-seek, pop a queued waypoint, or report the run already exhausted.
//============================================================================
#include "pathmove.h"

static inline float AsHardwareFloat(CFloat value) { return *(const float*)&value; }

int CPathClient::Classify(const CGridVec2* to, CGridVec2* from)
{
    CFloat distSq = CFloatDistanceSquared(*from, *to);
    CFloat thresholdSq = g_pathStepScale * g_pathStepScale;

    if (!(AsHardwareFloat(distSq) <= AsHardwareFloat(thresholdSq)))
    {
        ClearSearchState();
        return 0;
    }

    if (m_cursor[0] != 0)
    {
        ClearSearchState();
        SeekCachedPoint(to, from);
        return 0;
    }

    if (m_cursor[1] == 0)
        return 4;

    int moved;
    if (!PopWaypoint(1, &moved))
    {
        if (m_cooldown < 20)
            m_cooldown = 20;
    }

    m_cursor[0] = (unsigned int)-1;
    return 2;
}
