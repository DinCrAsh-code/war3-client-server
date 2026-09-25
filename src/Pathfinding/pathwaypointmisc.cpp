//============================================================================
//  0x6F49B920 / 0x6F49B5A0 - small CPathClient waypoint/lane helpers.
//============================================================================
#include "pathmove.h"

static inline float AsHardwareFloat(CFloat value) { return *(const float*)&value; }

//----------------------------------------------------------------------------
//  0x6F49B920 - TailWaypoint: the queued-order length and the second
//  cache's own cursor are the same field (m_cursor[1]) at different points
//  in its lifecycle - while an order is being queued it counts entries,
//  while the cache is being walked it indexes into it. Read here as a
//  count: fewer than two queued points has no "one before the last" to
//  report. The epsilon compare is an inequality test (MSVC's
//  `fucompp`/`test ah,44h`/`jp` spelling of `!=`) against the same span
//  epsilon TrimTail uses - the tail entry only has a real queued length
//  once its x has been stamped with that marker, so anything else there
//  reports "nothing queued" rather than a leftover garbage length. The
//  caller (`PopWaypoint`) only ever tests the low byte of the result
//  (`test al, al`), which is what the return type being narrower than
//  `int` buys: the zero path is `xor al, al`, not `xor eax, eax`.
//----------------------------------------------------------------------------
unsigned char CPathClient::TailWaypoint()
{
    unsigned int count = m_cursor[1];
    if (count < 2)
        return 0;

    CGridVec2* last = &m_cache[1].Points()[count - 1];
    if (AsHardwareFloat(last->m_x) != AsHardwareFloat(g_pathSpanEpsilon))
        return 0;

    return (unsigned char)CFloatToInt(last->m_y);
}

//----------------------------------------------------------------------------
//  0x6F49B5A0 - ReleaseLane: drop this client from whichever lane
//  CPathClient::Lane currently picks, and clear the "ran a search this
//  tick" flag that feeds that choice.
//----------------------------------------------------------------------------
void CPathClient::ReleaseLane()
{
    Lane()->Unlink(this);
    m_flags &= ~kPathFlagSearched;
}
