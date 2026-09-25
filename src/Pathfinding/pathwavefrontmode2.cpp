//============================================================================
//  0x6F4A2180 - word_6FAB774C mode 1 (2-cell radius): the direction-aware
//  neighbour scan, the same shape as TestFootprint2 (pathfootprint.cpp)
//  with VisitCellRun/Column/Rect standing in for TestCellRun/Column/Rect
//  and no short-circuiting.
//============================================================================
#include "pathfind.h"

void CPathSearch::VisitNeighbours2(const SCellPoint* pt, int dir,
                                   SPathFrontier* f)
{
    switch (dir)
    {
    case kPathDirSouth:                     //  1
        VisitCellRun(pt->m_x - 1, pt->m_y - 1, f, 2);
        return;

    case kPathDirEast:                      //  2
        VisitCellColumn(pt->m_x, pt->m_y - 1, f, 2);
        return;

    case kPathDirNorth:                     //  4
        VisitCellRun(pt->m_x - 1, pt->m_y, f, 2);
        return;

    case kPathDirWest:                      //  8
        VisitCellColumn(pt->m_x - 1, pt->m_y - 1, f, 2);
        return;

    case kPathDirSouth | kPathDirWest:      //  9
        VisitCellRun(pt->m_x - 1, pt->m_y - 1, f, 3);
        VisitCellColumn(pt->m_x - 1, pt->m_y, f, 2);
        return;

    case kPathDirSouth | kPathDirEast:      //  3
        VisitCellRun(pt->m_x - 2, pt->m_y - 1, f, 3);
        VisitCellColumn(pt->m_x, pt->m_y, f, 2);
        return;

    case kPathDirEast | kPathDirNorth:      //  6
        VisitCellRun(pt->m_x - 2, pt->m_y, f, 3);
        VisitCellColumn(pt->m_x, pt->m_y - 2, f, 2);
        return;

    case kPathDirNorth | kPathDirWest:      //  12
        VisitCellRun(pt->m_x - 1, pt->m_y, f, 3);
        VisitCellColumn(pt->m_x - 1, pt->m_y - 2, f, 2);
        return;
    }

    VisitCellRect(pt->m_x - 1, pt->m_y - 1, f, 2, 2);
}
