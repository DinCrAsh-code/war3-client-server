//============================================================================
//  0x6F4A3990 - "is `node` registered in this cell, reachably".
//
//  The bounds test and the cell fetch are CCellGrid::CellAt open-coded, the
//  same way every A* gate open-codes its own copy: one out-of-line body
//  shared between them would be one call where the shipped code has none.
//  The `cell != 0` test after it is the shipped code's own - it cannot fail
//  once the bounds test has passed, and it is still there.
//
//  Both of the caller's own guards are repeated here rather than assumed:
//  ScanRing passes the same `node` to every cell of the ring, so the null
//  and the -1 stamp are re-tested on each one.
//============================================================================
#include "pathfind.h"
#include "spatialgrid.h"

int CPathSearch::CellHasNode(int x, int y, const CGridRegistration* node)
{
    CCellGrid* g = m_grid;

    if ((unsigned int)x < (unsigned int)g->m_stride &&
        (unsigned int)y < (unsigned int)g->m_rows)
    {
        int* cell = g->m_cells + (g->m_stride * y + x);
        if (cell != 0 && node != 0 && node->m_visitStamp != -1 &&
            CellReaches(cell, node) != 0)
            return 1;
    }

    return 0;
}
