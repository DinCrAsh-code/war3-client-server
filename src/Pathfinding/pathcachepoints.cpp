//============================================================================
//  0x6F49C930 / 0x6F49C990 / 0x6F49C380 - the second point cache's own
//  cursor walk: hand back the next cached point, retire it, or seek ahead
//  to a fresh one when the cursor runs past the mover's own current
//  position slot.
//============================================================================
#include "pathmove.h"

//  0x6F49C930 - NextCachedPoint: same first-cache cursor SeekCachedPoint
//  uses (this+0x74/this+0x40), not the second cache TrimTail/PopWaypoint/
//  TailWaypoint share.
int CPathClient::NextCachedPoint(const CGridVec2* to, CGridVec2* out)
{
    if (m_cursor[0] >= m_cache[0].m_count)
    {
        CGridVec2 scratch;
        PointForSlot(&scratch, 0);
        if (!RunTrace(to, &scratch))
            return 0;
    }

    *out = m_cache[0].Points()[m_cursor[0]];
    return 1;
}

//  0x6F49C380 - SeekCachedPoint: advance the FIRST cache's own cursor via
//  FirstBlockedPoint and hand back the point it lands on. (An earlier
//  pass had this indexing m_cursor[1]/m_cache[1] - the second cache,
//  which TrimTail/PopWaypoint/TailWaypoint all use - but the disassembly
//  reads/writes this+0x74/this+0x40, the *first* cache's own cursor and
//  Points() base.)
void CPathClient::SeekCachedPoint(const CGridVec2* to, CGridVec2* out)
{
    m_cursor[0] = (unsigned int)FirstBlockedPoint(to);
    *out = m_cache[0].Points()[m_cursor[0]];
}

//  0x6F49C990 - RetireCachedPoint: once the queue actually has something
//  in it (m_cursor[1] != 0), pop one waypoint - after a CheckSlotDistance
//  gate unless `force` skips it - and clear the *first* cache's own
//  cursor once that succeeds. *movedOut from PopWaypoint doubles as this
//  function's own "did anything move" report.
int CPathClient::RetireCachedPoint(const CGridVec2* to, int force)
{
    int trimmed = 0;
    if (m_cursor[1] > 0)
    {
        int proceed = 1;
        if (force == 0)
            proceed = CheckSlotDistance(to, 1, &g_pathStepScale);

        if (proceed != 0)
        {
            if (!PopWaypoint(1, &trimmed))
            {
                if (m_cooldown < 0x14)
                    m_cooldown = 0x14;
                return 2;
            }
            m_cursor[0] = (unsigned int)-1;
        }
    }
    return trimmed != 0;
}
