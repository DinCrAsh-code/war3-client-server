//============================================================================
//  0x6F4991C0 - 0x6F499590 - the portal expander's own passability gates.
//
//  The per-cell expander has nine of these (pathastargates.cpp); the portal
//  expander has nine more, at their own addresses, and they are not the same
//  nine.  A portal step crosses a whole 2^level block rather than one cell,
//  so each direction needs two: an *interior* gate for a sample point that is
//  not on the block's far edge, and an *edge* gate for one that is, which has
//  to look at the cells on the other side of the boundary as well.  Which of
//  the two a span uses is decided per call by
//  `x < ((x >> level) << level) + (1 << level) - 1` (pathportalspans.cpp).
//
//  The four corner expanders take a single gate each, and the north-west
//  corner takes none at all - see pathportalcorners.cpp.
//
//  Three of the nine are the same body at three addresses (the plain
//  "is the cell east of here clear" test, 0x6F4991C0 / 0x6F499530, and the
//  "is the cell south of here clear" test, 0x6F4992A0 / 0x6F499590).  They
//  are separate functions in the shipped build and separate here.
//
//  The two south-facing ones name their `cy` before their `cx`, for the same
//  reason pathastargates.cpp's two half-cell gates do: the shipped body reads
//  arg_4 into the first callee-saved register it takes.  0x6F4994A0 is the
//  exception that proves it - it is also south-facing but reads arg_0 first,
//  and so names `cx` first here.
//
//  PortalGateEast lives in its own translation unit (pathportalgateeast.cpp)
//  because 0x6F499380 and 0x6F499410 call it for real and MSVC would
//  otherwise fold it into both.
//============================================================================
#include "pathastar.h"

//----------------------------------------------------------------------------
//  0x6F4991C0 - the north span's interior gate: the cell east of the sample
//  point has to be clear.
//----------------------------------------------------------------------------
int CPathAStar::PortalGateNorth(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    int cx = x + 1;
    int cy = y;
    return ASTAR_CELL_OPEN(g, cx, cy);
}

//----------------------------------------------------------------------------
//  0x6F4992F0 - the north span's edge gate.  The cell east of the sample
//  point and the one south-east of it, and then either a second cell two
//  south of that or the cell west of the sample point - the block boundary
//  can be crossed either by going round it or by stepping back off it.
//----------------------------------------------------------------------------
int CPathAStar::PortalGateNorthEdge(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    int cx = x + 1;
    int cy = y;
    return ASTAR_CELL_OPEN(g, cx, cy)
        && IsCellOpen(x + 1, y + 1)
        && (IsCellOpen(x + 1, y + 2) || IsCellOpen(x - 1, y));
}

//----------------------------------------------------------------------------
//  0x6F499380 - the east span's edge gate: the interior gate, and then any
//  one of three pairs of cells around the boundary.
//----------------------------------------------------------------------------
int CPathAStar::PortalGateEastEdge(int x, int y)
{
    return PortalGateEast(x, y)
        && ((IsCellOpen(x, y - 1)     && IsCellOpen(x + 1, y - 1))
         || (IsCellOpen(x - 2, y + 1) && IsCellOpen(x - 1, y + 1))
         || (IsCellOpen(x - 1, y + 1) && IsCellOpen(x, y - 1)));
}

//----------------------------------------------------------------------------
//  0x6F499410 - the south span's edge gate: the same three pairs as the east
//  one, reflected about the diagonal.
//----------------------------------------------------------------------------
int CPathAStar::PortalGateSouthEdge(int x, int y)
{
    return PortalGateEast(x, y)
        && ((IsCellOpen(x - 1, y)     && IsCellOpen(x - 1, y + 1))
         || (IsCellOpen(x + 1, y - 2) && IsCellOpen(x + 1, y - 1))
         || (IsCellOpen(x + 1, y - 1) && IsCellOpen(x - 1, y)));
}

//----------------------------------------------------------------------------
//  0x6F4992A0 - the west span's interior gate: the cell south of the sample
//  point has to be clear.
//----------------------------------------------------------------------------
int CPathAStar::PortalGateWest(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    int cy = y + 1;
    int cx = x;
    return ASTAR_CELL_OPEN(g, cx, cy);
}

//----------------------------------------------------------------------------
//  0x6F4994A0 - the west span's edge gate, the mirror of 0x6F4992F0.  Names
//  `cx` first: this one reads arg_0 into the first register it takes.
//----------------------------------------------------------------------------
int CPathAStar::PortalGateWestEdge(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    int cx = x;
    int cy = y + 1;
    return ASTAR_CELL_OPEN(g, cx, cy)
        && IsCellOpen(x + 1, y + 1)
        && (IsCellOpen(x + 2, y + 1) || IsCellOpen(x, y - 1));
}

//----------------------------------------------------------------------------
//  0x6F499530 - the north-east corner's gate.  Byte-for-byte the same test
//  as PortalGateNorth, at its own address.
//----------------------------------------------------------------------------
int CPathAStar::PortalGateNorthEastCorner(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    int cx = x + 1;
    int cy = y;
    return ASTAR_CELL_OPEN(g, cx, cy);
}

//----------------------------------------------------------------------------
//  0x6F499590 - the south-west corner's gate, likewise a second copy of
//  PortalGateWest.
//----------------------------------------------------------------------------
int CPathAStar::PortalGateSouthWestCorner(int x, int y)
{
    SAStarGrid* g = m_grids[0];
    int cy = y + 1;
    int cx = x;
    return ASTAR_CELL_OPEN(g, cx, cy);
}
