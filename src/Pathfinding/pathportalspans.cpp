//============================================================================
//  0x6F4998F0 / 0x6F4999F0 / 0x6F499AF0 / 0x6F499BF0 - the four span testers
//  the portal expander walks its block's edges with.
//
//  A portal node covers a 2^level block of cells, so its "north neighbour" is
//  not one cell but a whole row of them.  Each of these four takes the sample
//  point on one edge and resolves it to a node:
//
//    * a real node index - relax it, and report both ends of the span as
//      reachable so the two corners flanking it may be tried;
//    * -2 - the cell belongs to a *finer* block than `level`, so the span has
//      to be split: recurse on the low half at level-1 and loop on the high
//      half, which is what the shipped code's jump back to the -2 test is;
//    * anything else (-1) - blocked, nothing to do.
//
//  The two out-parameters are the low and high ends of the span, and only the
//  outermost sub-span at each end may write its own: after the recursive call
//  the low pointer is overwritten with `&g_astarSpanSink`, a discard global,
//  so every sub-span but the leftmost writes its "low" flag into the bin.
//  The high pointer needs no such trick because the loop *is* the high half -
//  it is the last iteration that writes it.  That is the whole reason the
//  shipped code passes `offset unk_6FAB7744` twice.
//
//  `if (level == 0) return;` before the two stores is not a guard against
//  anything: a span that bottomed out at a single cell does not span the
//  corner, so neither corner flanking it is offered the diagonal.
//
//  The gate pair is chosen per call: a sample point that is not on the far
//  edge of its own block takes the interior gate, one that is takes the edge
//  gate (pathportalgates.cpp).
//
//  All four are the same function about a different axis.  North and south
//  subdivide along x, east and west along y - which is why the north/south
//  pair read arg_C into `eax` and arg_8 into `edi` and the east/west pair do
//  the opposite.
//============================================================================
#include "pathastar.h"

//----------------------------------------------------------------------------
//  0x6F4998F0 - the row of cells immediately north of the block.
//----------------------------------------------------------------------------
void CPathAStar::SpanNorth(int node, int level, int x, int y,
                           int* outLo, int* outHi)
{
    int found = LookupNode(level, x, y);

    while ((unsigned int)found >= 0xFFFFFFFEu)
    {
        if ((unsigned int)found != 0xFFFFFFFEu)
            return;

        int base = (x >> level) << level;
        --level;
        int mid = (1 << level) + base;
        int lo;
        if (x < mid)
        {
            lo = x;
            x  = mid;
        }
        else
        {
            lo = mid - (level != 0 ? m_mode : 1);
        }

        SpanNorth(node, level, lo, y, outLo, &g_astarSpanSink);
        outLo = &g_astarSpanSink;
        found = LookupNode(level, x, y);
    }

    if (m_mode == 2)
    {
        int last = ((x >> level) << level) + (1 << level) - 1;
        int ok;
        if (x < last)
            ok = PortalGateNorth(x, y);
        else
            ok = PortalGateNorthEdge(x, y);
        if (!ok)
            return;
    }

    Relax(found, node);

    if (level == 0)
        return;

    *outHi = 1;
    *outLo = 1;
}

//----------------------------------------------------------------------------
//  0x6F4999F0 - the column of cells immediately east of the block.
//----------------------------------------------------------------------------
void CPathAStar::SpanEast(int node, int level, int x, int y,
                          int* outLo, int* outHi)
{
    int found = LookupNode(level, x, y);

    while ((unsigned int)found >= 0xFFFFFFFEu)
    {
        if ((unsigned int)found != 0xFFFFFFFEu)
            return;

        int base = (y >> level) << level;
        --level;
        int mid = (1 << level) + base;
        int lo;
        if (y < mid)
        {
            lo = y;
            y  = mid;
        }
        else
        {
            lo = mid - (level != 0 ? m_mode : 1);
        }

        SpanEast(node, level, x, lo, outLo, &g_astarSpanSink);
        outLo = &g_astarSpanSink;
        found = LookupNode(level, x, y);
    }

    if (m_mode == 2)
    {
        int last = ((y >> level) << level) + (1 << level) - 1;
        int ok;
        if (y < last)
            ok = PortalGateEast(x, y);
        else
            ok = PortalGateEastEdge(x, y);
        if (!ok)
            return;
    }

    Relax(found, node);

    if (level == 0)
        return;

    *outHi = 1;
    *outLo = 1;
}

//----------------------------------------------------------------------------
//  0x6F499AF0 - the row of cells immediately south of the block.
//----------------------------------------------------------------------------
void CPathAStar::SpanSouth(int node, int level, int x, int y,
                           int* outLo, int* outHi)
{
    int found = LookupNode(level, x, y);

    while ((unsigned int)found >= 0xFFFFFFFEu)
    {
        if ((unsigned int)found != 0xFFFFFFFEu)
            return;

        int base = (x >> level) << level;
        --level;
        int mid = (1 << level) + base;
        int lo;
        if (x < mid)
        {
            lo = x;
            x  = mid;
        }
        else
        {
            lo = mid - (level != 0 ? m_mode : 1);
        }

        SpanSouth(node, level, lo, y, outLo, &g_astarSpanSink);
        outLo = &g_astarSpanSink;
        found = LookupNode(level, x, y);
    }

    if (m_mode == 2)
    {
        int last = ((x >> level) << level) + (1 << level) - 1;
        int ok;
        if (x < last)
            ok = PortalGateEast(x, y);
        else
            ok = PortalGateSouthEdge(x, y);
        if (!ok)
            return;
    }

    Relax(found, node);

    if (level == 0)
        return;

    *outHi = 1;
    *outLo = 1;
}

//----------------------------------------------------------------------------
//  0x6F499BF0 - the column of cells immediately west of the block.
//----------------------------------------------------------------------------
void CPathAStar::SpanWest(int node, int level, int x, int y,
                          int* outLo, int* outHi)
{
    int found = LookupNode(level, x, y);

    while ((unsigned int)found >= 0xFFFFFFFEu)
    {
        if ((unsigned int)found != 0xFFFFFFFEu)
            return;

        int base = (y >> level) << level;
        --level;
        int mid = (1 << level) + base;
        int lo;
        if (y < mid)
        {
            lo = y;
            y  = mid;
        }
        else
        {
            lo = mid - (level != 0 ? m_mode : 1);
        }

        SpanWest(node, level, x, lo, outLo, &g_astarSpanSink);
        outLo = &g_astarSpanSink;
        found = LookupNode(level, x, y);
    }

    if (m_mode == 2)
    {
        int last = ((y >> level) << level) + (1 << level) - 1;
        int ok;
        if (y < last)
            ok = PortalGateWest(x, y);
        else
            ok = PortalGateWestEdge(x, y);
        if (!ok)
            return;
    }

    Relax(found, node);

    if (level == 0)
        return;

    *outHi = 1;
    *outLo = 1;
}
