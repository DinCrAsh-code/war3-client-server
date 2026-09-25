//============================================================================
//  0x6F498890 - "is this cell passable for the unit this search is for", as
//  a function of its own.
//
//  The nine gates in pathastargates.cpp each carry an open-coded copy of the
//  same test and call this one only for a second cell - see ASTAR_CELL_OPEN
//  in pathastar.h for why that duplication is spelled the way it is.
//
//  Own translation unit: next to the gates, MSVC folds their calls to it away
//  and none of them match.
//============================================================================
#include "pathastar.h"

int CPathAStar::IsCellOpen(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    return ASTAR_CELL_OPEN(g, x, y);
}
