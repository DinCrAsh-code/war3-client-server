//============================================================================
//  0x6F4A1160 / 0x6F4A11A0 / 0x6F4A11E0 - runs of cells.
//
//  A row, a column, and a rectangle built out of rows.  All three stop at
//  the first cell TestCell rejects and return 1 only if every cell passed;
//  an empty run passes.
//
//  The loop bounds are signed - the end is `start + count` and the guard is
//  `jge`/`jl` - while TestCell's own bounds check is unsigned, so a run that
//  starts left of or below the grid is rejected cell by cell rather than
//  clipped.  See docs/msvc-vc8-idioms.md, "Loops that are signed at the
//  guard and unsigned in the body".
//
//  Own translation unit: the footprint tests in pathfootprint.cpp call all
//  three for real.  TestCellRect calling TestCellRun from inside this same
//  unit is safe - MSVC does not inline a function containing a loop.
//============================================================================
#include "pathfind.h"

//  0x6F4A1160 - `count` cells starting at (x, y), walking +x.
int CPathSearch::TestCellRun(int x, int y, int count)
{
    int end = x + count;
    for (int i = x; i < end; i++)
    {
        if (!TestCell((unsigned int)i, (unsigned int)y))
            return 0;
    }
    return 1;
}

//  0x6F4A11A0 - `count` cells starting at (x, y), walking +y.
int CPathSearch::TestCellColumn(int x, int y, int count)
{
    int end = y + count;
    for (int j = y; j < end; j++)
    {
        if (!TestCell((unsigned int)x, (unsigned int)j))
            return 0;
    }
    return 1;
}

//  0x6F4A11E0 - a `width` x `height` block with its low corner at (x, y).
int CPathSearch::TestCellRect(int x, int y, int width, int height)
{
    int end = y + height;
    for (int j = y; j < end; j++)
    {
        if (!TestCellRun(x, j, width))
            return 0;
    }
    return 1;
}
