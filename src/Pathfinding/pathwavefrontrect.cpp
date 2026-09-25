//============================================================================
//  0x6F4A2020 - VisitCell's rect span: `height` rows of VisitCellRun.  Its
//  own translation unit so the call into pathwavefrontspan.cpp's
//  VisitCellRun cannot be inlined away - a void, single-call loop body
//  inlines readily at /O2 where the placement search's own TestCellRun
//  (which returns a value the caller branches on) does not, so the
//  precedent in pathspan.cpp of sharing one TU does not carry over here.
//============================================================================
#include "pathfind.h"

void CPathSearch::VisitCellRect(int x, int y, SPathFrontier* frontier,
                                int width, int height)
{
    int end = y + height;
    for (int j = y; j < end; j++)
        VisitCellRun(x, j, frontier, width);
}
