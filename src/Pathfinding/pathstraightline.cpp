//============================================================================
//  0x6F49DAC0 - CPathClient::TryStraightLine: the fast path Advance takes
//  before committing to a full search.
//
//  Both points this works in are grid-space: the order's destination and
//  whatever PointForSlot's slot 1 currently holds, each scaled by the shared
//  grid's cells-per-world-unit.  The scaling is written out here rather than
//  through ScaleGridVec2 (pathwaypointpop.cpp) because the shipped code has
//  it inlined - two CFloat::operator* calls, y before x, which is the
//  argument evaluation order a `CGridVec2(x * s, y * s)` constructor already
//  produces.
//
//  The work only happens when the queued run is exhausted (the cursor has
//  reached the second cache's count).  From there the two arms are quite
//  different:
//
//    * with flag 0x200000 set, a real query is run for the slot point, and a
//      failed query is reported to the caller as 2 - which Advance passes
//      straight back out, so 2 is "no route, and do not fall through to the
//      full search".  A successful one consumes one waypoint.
//    * without it, the cache is simply thrown away and re-seeded with that
//      one point, and the cursor rewound.
//
//  Either way the tail is the same RetireCachedPoint call, and its result is
//  what this returns - so a client whose cursor is still inside a queued run
//  does nothing here but that.
//============================================================================
#include "pathmove.h"
#include "pathfind.h"
#include "spatialgrid.h"

int CPathClient::TryStraightLine(const CGridVec2* to)
{
    //  A copy, not a reference: the shipped code parks the scale in a stack
    //  slot of its own and hands both multiplies that slot's address.
    CFloat scale(PathOwnerSingleton()->m_grid->m_cellScale.m_bits);

    CGridVec2 dest(to->m_x * scale, to->m_y * scale);

    //  kCFloatNoInit and the *returned* pointer, not the local's own
    //  address: the shipped code neither zeroes this pair before filling it
    //  nor re-forms its address afterwards - it keeps PointForSlot's return
    //  value in edi and reads both components through that.
    CGridVec2 slot(kCFloatNoInit);
    const CGridVec2* slotPt = PointForSlot(&slot, 1);
    CGridVec2 slotScaled(slotPt->m_x * scale, slotPt->m_y * scale);

    //  Written count-first, not cursor-first.  Both spellings mean the
    //  same thing, but MSVC keeps the left-hand operand as the `cmp`'s
    //  first operand and loads whichever side it must: with the cursor on
    //  the left it emits `cmp [cursor], reg` and leaves the count in a
    //  register, which then gets reused in the reseed arm below instead of
    //  being re-read the way the shipped code re-reads it.  This way round
    //  costs the three instructions of a `ja` where the shipped code has a
    //  `jb` and buys back nine.
    unsigned int cursor = m_cursor[1];
    if (m_cache[1].m_count <= cursor)
    {
        if ((m_flags & 0x200000) != 0)
        {
            if (RunPathQuery(&dest, &slotScaled, 1) == 0)
                return 2;

            int moved;
            PopWaypoint(0, &moved);
        }
        else
        {
            //  Bound once: the shipped code forms &m_cache[1] into edi
            //  ahead of the emptiness test and reaches both calls through
            //  it rather than re-forming the address for each.
            SPathPointCache* cache = &m_cache[1];
            unsigned int queued = cache->m_count;
            if (queued != 0)
                cache->EraseRange(0, queued);

            cache->Grow(&slotScaled, 1);
            m_cursor[1] = 0;
        }
    }

    return RetireCachedPoint(&dest, 0);
}
