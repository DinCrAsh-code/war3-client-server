//============================================================================
//  0x6F499FA0 - expand a portal node's neighbours.
//
//  The other expander (pathastarexpand.cpp) walks eight cells; this one walks
//  the four *edges* of a 2^level block and its four corners.  `level` is the
//  node's own m_type, so the block is the one the node stands for and
//  (x, y) is the cell inside it the search actually reached.
//
//  x0/y0 are that cell snapped down to the block's own origin - `(x >> level)
//  << level` - and `size` is 1 << level, so the four spans run one cell
//  outside the block on each side:  y0-1 north, x0+size east, y0+size south,
//  x0-1 west.  The other coordinate of each span is the sample point itself,
//  which is what makes the search prefer to leave a block near where it
//  entered it.
//
//  At mode 2 the sample point is first pulled one cell in from the block's
//  far edges.  That is not a bounds clamp - x and y are already inside the
//  block - it is the mode-2 gates needing a whole cell of room on the far
//  side of whatever they test.
//
//  Each span writes two flags, its low end and its high end, and a corner
//  runs only when both spans that meet there reported that end reachable.
//  Which flag is "low" and which is "high" is by coordinate, not by compass:
//  for the two horizontal spans low is west and high is east, for the two
//  vertical ones low is north and high is south.  So the north-east corner
//  wants north's *high* flag and east's *low* one, and so on round the four.
//
//  The eight locals are declared in the order the shipped code zeroes them.
//============================================================================
#include "pathastar.h"

void CPathAStar::ExpandPortal(int node, int level, int x, int y)
{
    int size = 1 << level;
    int x0 = (x >> level) << level;
    int y0 = (y >> level) << level;

    int westLo = 0, westHi = 0;
    int southLo = 0, southHi = 0;
    int eastHi = 0, eastLo = 0;
    int northHi = 0, northLo = 0;

    if (m_mode == 2)
    {
        int xMax = x0 + size - 2;
        int yMax = y0 + size - 2;
        if (x > xMax)
            x = xMax;
        if (y > yMax)
            y = yMax;
    }

    int northY = y0 - 1;

    SpanNorth(node, level, x, northY, &northLo, &northHi);
    SpanEast(node, level, x0 + size, y, &eastLo, &eastHi);
    SpanSouth(node, level, x, y0 + size, &southLo, &southHi);
    SpanWest(node, level, x0 - 1, y, &westLo, &westHi);

    if (northHi && eastLo)
        PortalNorthEast(node, level, x0 + size, northY);
    if (eastHi && southHi)
        PortalSouthEast(node, level, x0 + size, y0 + size);
    if (southLo && westHi)
        PortalSouthWest(node, level, x0 - 1, y0 + size);
    if (westLo && northLo)
        PortalNorthWest(node, level, x0 - 1, northY);
}
