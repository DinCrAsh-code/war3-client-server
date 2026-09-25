//============================================================================
//  0x6F499E90 - expand a node's eight grid neighbours.
//
//  The four cardinals are always tested.  A diagonal is tested only when both
//  of the cardinals flanking it came back passable, which is the standard
//  no-cutting-through-a-corner rule: a unit may not slip between two blocked
//  cells that meet at the corner it is trying to cross.
//
//  MSVC chains the four `&&` tests rather than compiling them independently -
//  a failed `east` makes both "north and east" and "east and south" false, so
//  it jumps past the second test entirely.  That is why the shipped code's
//  branch targets skip two conditions at a time and why the four statements
//  below have to stay in this order.
//
//  The tail is a ninth, non-grid neighbour: a node can carry a link slot
//  index into a side table of extra edges (a portal, a ramp - whatever the
//  map author connected by hand), and if that slot is live and does not just
//  point back at this same cell, its far end is expanded too.
//============================================================================
#include "pathastar.h"

void CPathAStar::ExpandCell(int node, int x, int y, int linkSlot)
{
    int north = TestNorth(node, x, y - 1);
    int east  = TestEast(node, x + 1, y);
    int south = TestSouth(node, x, y + 1);
    int west  = TestWest(node, x - 1, y);

    if (north && east)
        TestNorthEast(node, x + 1, y - 1);
    if (east && south)
        TestSouthEast(node, x + 1, y + 1);
    if (south && west)
        TestSouthWest(node, x - 1, y + 1);
    if (west && north)
        TestNorthWest(node, x - 1, y - 1);

    if (linkSlot != 0)
    {
        SAStarLink* link = &m_links[linkSlot];
        if ((link->m_flags & 1) != 0)
        {
            if (x != link->m_x || y != link->m_y)
                TestLink(node, link->m_x, link->m_y);
        }
    }
}
