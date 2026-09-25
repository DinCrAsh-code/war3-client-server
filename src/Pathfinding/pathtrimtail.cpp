//============================================================================
//  0x6F49C100 - CPathClient::TrimTail: walk the second cache's queued
//  waypoints backward from the tail, accumulating segment length, and
//  report how far back the walk got before the running total caught up
//  with g_pathTrimEpsilon.
//
//  A waypoint whose x equals the span epsilon is a sentinel marking a gap
//  in the queue rather than a real point; hitting one stops the walk
//  immediately and reports it through *outFlag, returning one slot short
//  or past it depending on whether the caller wants to keep the last real
//  point (`keepLast`). Otherwise each step adds this segment's length to a
//  running total (seeded at g_CFloatZero, never reset) and keeps stepping
//  back while that *cumulative* distance is still under g_pathTrimEpsilon -
//  a single long segment can stop the walk on its own, but so can enough
//  short ones added together.
//============================================================================
#include "pathmove.h"

static inline float AsHardwareFloat(CFloat value) { return *(const float*)&value; }

int CPathClient::TrimTail(int* outFlag, int keepLast)
{
    *outFlag = 0;
    unsigned int count = m_cursor[1];
    if (count == 0)
        return 0;

    int index = (int)count - 1;
    CFloat accum = g_CFloatZero;
    if (index <= 0)
        return index;

    for (;;)
    {
        CGridVec2* p = &m_cache[1].Points()[index];
        if (AsHardwareFloat(p->m_x) == AsHardwareFloat(g_pathSpanEpsilon))
        {
            *outFlag = 1;
            return keepLast ? index - 1 : index + 1;
        }

        CGridVec2* next = p + 1;
        __declspec(align(8)) CFloat delta[2] = { CFloat(kCFloatNoInit), CFloat(kCFloatNoInit) };
        delta[1] = next->m_y - p->m_y;
        delta[0] = next->m_x - p->m_x;
        accum = accum + CFloatSqrt(delta[1] * delta[1] + delta[0] * delta[0]);

        if (!(AsHardwareFloat(accum) < AsHardwareFloat(g_pathTrimEpsilon)))
            break;

        index -= 1;
        if (index == 0)
            break;
    }

    return index;
}
