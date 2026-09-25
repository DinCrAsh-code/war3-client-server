//============================================================================
//  0x6F499210 - the portal expander's most-used gate: the cell east of the
//  sample point, plus the two cells of the row below it.
//
//  Three call sites want it - the east span's interior test, the south
//  span's interior test and the south-east corner's only gate - and two more
//  (0x6F499380, 0x6F499410, both in pathportalgates.cpp) call it as the first
//  half of their own.  Its own translation unit so those two calls survive.
//============================================================================
#include "pathastar.h"

int CPathAStar::PortalGateEast(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    int cx = x + 1;
    int cy = y;
    return ASTAR_CELL_OPEN(g, cx, cy)
        && IsCellOpen(x, y + 1)
        && IsCellOpen(x + 1, y + 1);
}
