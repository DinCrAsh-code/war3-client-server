//============================================================================
//  0x6F4A2360 - word_6FAB774C mode 2 (3-cell radius): the direction-aware
//  neighbour scan for the coarse search, exactly the shape TestFootprint3
//  uses (pathfootprint.cpp) with VisitCellRun/Column/Rect standing in for
//  TestCellRun/Column/Rect and no short-circuiting - VisitCell has nothing
//  to stop early on, so each `&&`-chained pair there is two unconditional
//  calls here.
//============================================================================
#include "pathfind.h"

void CPathSearch::VisitNeighbours3(const SCellPoint* pt, int dir,
                                   SPathFrontier* f)
{
    switch (dir)
    {
    case kPathDirSouth:                     //  1
        VisitCellRun(pt->m_x - 1, pt->m_y - 1, f, 3);
        return;

    case kPathDirEast:                      //  2
        VisitCellColumn(pt->m_x + 1, pt->m_y - 1, f, 3);
        return;

    case kPathDirNorth:                     //  4
        VisitCellRun(pt->m_x - 1, pt->m_y + 1, f, 3);
        return;

    case kPathDirWest:                      //  8
        VisitCellColumn(pt->m_x - 1, pt->m_y - 1, f, 3);
        return;

    case kPathDirSouth | kPathDirWest:      //  9
        VisitCellRun(pt->m_x - 1, pt->m_y - 1, f, 4);
        VisitCellColumn(pt->m_x - 1, pt->m_y, f, 3);
        return;

    case kPathDirSouth | kPathDirEast:      //  3
        VisitCellRun(pt->m_x - 2, pt->m_y - 1, f, 4);
        VisitCellColumn(pt->m_x + 1, pt->m_y, f, 3);
        return;

    case kPathDirEast | kPathDirNorth:      //  6
        VisitCellRun(pt->m_x - 2, pt->m_y + 1, f, 4);
        VisitCellColumn(pt->m_x + 1, pt->m_y - 2, f, 3);
        return;

    case kPathDirNorth | kPathDirWest:      //  12
        VisitCellRun(pt->m_x - 1, pt->m_y + 1, f, 4);
        VisitCellColumn(pt->m_x - 1, pt->m_y - 2, f, 3);
        return;
    }

    VisitCellRect(pt->m_x - 1, pt->m_y - 1, f, 3, 3);
}
