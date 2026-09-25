//============================================================================
//  0x6F4A2070 - word_6FAB774C mode 0 (1-cell radius): visit `pt` itself,
//  plus - only on a diagonal move - the two cells that close the corner the
//  move would otherwise cut, the same "leading edges plus the corner"
//  shape TestFootprint1 uses (pathfootprint.cpp).  An axis move or an
//  invalid combination visits nothing beyond the centre cell.
//
//  Own translation unit: a void, single-call switch case inlines a callee
//  as readily as a single-call loop body does (see pathwavefrontrect.cpp),
//  and VisitCell must stay a real call here to match the shipped call graph.
//============================================================================
#include "pathfind.h"

void CPathSearch::VisitNeighbours1(const SCellPoint* pt, int dir,
                                   SPathFrontier* f)
{
    VisitCell(pt->m_x, pt->m_y, f);

    switch (dir)
    {
    case kPathDirSouth | kPathDirWest:      //  9
        VisitCell(pt->m_x + 1, pt->m_y, f);
        VisitCell(pt->m_x, pt->m_y + 1, f);
        return;

    case kPathDirSouth | kPathDirEast:      //  3
        VisitCell(pt->m_x - 1, pt->m_y, f);
        VisitCell(pt->m_x, pt->m_y + 1, f);
        return;

    case kPathDirEast | kPathDirNorth:      //  6
        VisitCell(pt->m_x - 1, pt->m_y, f);
        VisitCell(pt->m_x, pt->m_y - 1, f);
        return;

    case kPathDirNorth | kPathDirWest:      //  12
        VisitCell(pt->m_x + 1, pt->m_y, f);
        VisitCell(pt->m_x, pt->m_y - 1, f);
        return;
    }
}
