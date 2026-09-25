//============================================================================
//  0x6F4A39F0 - walk the perimeter of a square looking for a cell that
//  `node` reaches.
//
//  The square is `side` cells on a side with its north-west corner `radius`
//  cells up and left of (x, y), and the walk goes clockwise: east along the
//  top row, south down the right column, west along the bottom row, north up
//  the left column.  It stops on the first cell CellHasNode accepts.
//
//  The four loops share their two cursors rather than recomputing corners,
//  which is why each one is followed by a step that looks like an off-by-one
//  and is not: the row loop leaves x one past the corner, so the column loop
//  steps it back before it starts, and so on round the square.  Only two
//  bounds are ever stored - the row's own start and the column's - because
//  the other two are reachable from them.
//
//  Its own translation unit: CellHasNode (pathcellhasnode.cpp) is a real
//  call at all four sites in the shipped code.
//============================================================================
#include "pathfind.h"

int CPathSearch::ScanRing(int x, int y, const CGridRegistration* node,
                          int radius, int side)
{
    x -= radius;
    y -= radius;

    int firstX = x;
    int lastX = x + side;
    for (; x < lastX; ++x)
        if (CellHasNode(x, y, node))
            return 1;

    int lastY = y + side;
    ++y;
    --x;
    int firstY = y;
    for (; y < lastY; ++y)
        if (CellHasNode(x, y, node))
            return 1;

    --x;
    --y;
    for (; x >= firstX; --x)
        if (CellHasNode(x, y, node))
            return 1;

    --y;
    ++x;
    for (; y >= firstY; --y)
        if (CellHasNode(x, y, node))
            return 1;

    return 0;
}
