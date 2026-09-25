//============================================================================
//  0x6F4A1FA0 / 0x6F4A1FE0 - VisitCell's own row/column span iterators.
//  Same shape as the placement search's TestCellRun/TestCellColumn
//  (pathspan.cpp), with the frontier threaded through as an extra
//  argument instead of a boolean short-circuit - VisitCell has nothing
//  to stop early on.  VisitCellRect (the third of this family) is in its
//  own translation unit - see pathwavefrontrect.cpp for why.
//============================================================================
#include "pathfind.h"

void CPathSearch::VisitCellRun(int x, int y, SPathFrontier* frontier,
                               int count)
{
    int end = x + count;
    for (int i = x; i < end; i++)
        VisitCell(i, y, frontier);
}

void CPathSearch::VisitCellColumn(int x, int y, SPathFrontier* frontier,
                                  int count)
{
    int end = y + count;
    for (int j = y; j < end; j++)
        VisitCell(x, j, frontier);
}
