//============================================================================
//  0x6F498E80 - 0x6F499170 - the nine mode-2 passability gates.
//
//  Every one is one or two cell tests around the step being considered.  The
//  first is always an open-coded ASTAR_CELL_OPEN (pathastar.h); a second,
//  when there is one, is a call to CPathAStar::IsCellOpen
//  (pathastarcellopen.cpp).  That is exactly what the shipped build has -
//  one open-coded test per gate plus at most one call - and pathastar.h says
//  why it cannot come from a shared inline helper.
//
//  The two half-cell gates name their y before their x, the other seven the
//  other way round.  That is not a slip: the shipped pair read arg_4 into the
//  first callee-saved register they take and arg_0 into the second, and
//  naming them in the other order swaps the two registers through both
//  bodies.
//
//  Two of the nine are `||` rather than `&&`: north's and west's half-cell
//  gates pass if *either* of two cells is clear, because a step across a
//  half-cell boundary has two ways through and only needs one of them.  The
//  other seven require every cell they name.
//
//  GateNorth and GateNorthEast test the same cell, as do GateWest and
//  GateSouthWest.  They are still four separate functions in the shipped
//  build and four here.
//
//  The offset cell is named into a pair of locals before the test rather
//  than written into it: the shipped code forms both coordinates before its
//  first bounds compare, and leaving them in the macro's arguments defers the
//  second one past that compare and swaps two registers with it.
//
//  Own translation unit, separate from CPathAStar::IsCellOpen: next to it
//  MSVC folds the calls away and none of these match.
//============================================================================
#include "pathastar.h"

//----------------------------------------------------------------------------
//  0x6F498E80 - the north cardinal.
//----------------------------------------------------------------------------
int CPathAStar::GateNorth(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    int cx = x + 1;
    int cy = y;
    return ASTAR_CELL_OPEN(g, cx, cy);
}

//----------------------------------------------------------------------------
//  0x6F498ED0 - the east cardinal.
//----------------------------------------------------------------------------
int CPathAStar::GateEast(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    int cx = x + 1;
    int cy = y;
    return ASTAR_CELL_OPEN(g, cx, cy)
        && IsCellOpen(x + 1, y + 1);
}

//----------------------------------------------------------------------------
//  0x6F498F30 - the south cardinal.
//----------------------------------------------------------------------------
int CPathAStar::GateSouth(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    int cx = x;
    int cy = y + 1;
    return ASTAR_CELL_OPEN(g, cx, cy)
        && IsCellOpen(x + 1, y + 1);
}

//----------------------------------------------------------------------------
//  0x6F498F90 - the west cardinal.
//----------------------------------------------------------------------------
int CPathAStar::GateWest(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    int cx = x;
    int cy = y + 1;
    return ASTAR_CELL_OPEN(g, cx, cy);
}

//----------------------------------------------------------------------------
//  0x6F498FE0 - north's half-cell step.
//----------------------------------------------------------------------------
int CPathAStar::GateNorthHalf(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    int cy = y - 2;
    int cx = x + 1;
    return ASTAR_CELL_OPEN(g, cx, cy)
        || IsCellOpen(x - 1, y);
}

//----------------------------------------------------------------------------
//  0x6F499050 - west's half-cell step.
//----------------------------------------------------------------------------
int CPathAStar::GateWestHalf(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    int cy = y + 1;
    int cx = x - 2;
    return ASTAR_CELL_OPEN(g, cx, cy)
        || IsCellOpen(x, y - 1);
}

//----------------------------------------------------------------------------
//  0x6F4990C0 - the north-east diagonal.
//----------------------------------------------------------------------------
int CPathAStar::GateNorthEast(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    int cx = x + 1;
    int cy = y;
    return ASTAR_CELL_OPEN(g, cx, cy);
}

//----------------------------------------------------------------------------
//  0x6F499110 - the south-east diagonal.
//----------------------------------------------------------------------------
int CPathAStar::GateSouthEast(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    int cx = x + 1;
    int cy = y + 1;
    return ASTAR_CELL_OPEN(g, cx, cy);
}

//----------------------------------------------------------------------------
//  0x6F499170 - the south-west diagonal.
//----------------------------------------------------------------------------
int CPathAStar::GateSouthWest(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    int cx = x;
    int cy = y + 1;
    return ASTAR_CELL_OPEN(g, cx, cy);
}

