//============================================================================
//  0x6F4A1230 / 0x6F4A1350 / 0x6F4A14D0 / 0x6F4A1660 - does a footprint fit?
//
//  One function per size class: 1x1, 2x2, 3x3 and 4x4 cells.  Each takes the
//  cell the footprint is anchored on and, optionally, the direction the unit
//  is moving in.  With no direction (the only case this call tree reaches -
//  CPathSearch::TestFootprint always passes 0) the whole footprint is
//  tested.  With one, only the cells the move brings newly under the
//  footprint are: the leading edge for an axis move, and for a diagonal the
//  two leading edges as one over-long row plus one column, which is the same
//  L-shape written with the corner counted once.
//
//  The direction is a bitmask - 1 = -y, 2 = +x, 4 = +y, 8 = -x - so the four
//  diagonals are 3, 6, 9 and 12.  The opposite pairs 5 and 10, everything
//  with three or more bits, and 0 are not moves and fall to the full test.
//  For the 1x1 case the "full test" is the anchor cell alone, which is why
//  its switch has no work to do in the default case and simply returns the
//  result of the test it already made.
//
//  The anchor is the footprint's *high* corner: the 2x2 shape covers
//  (x-1..x, y-1..y), the 3x3 (x-1..x+1, y-1..y+1) and the 4x4
//  (x-2..x+1, y-2..y+1).  That is recovered from each default case, and
//  every directional case is consistent with it.
//
//  The `pt` fields are re-read between every call because TestCell can
//  write memory; that is why the shipped code reloads [esi] and [esi+4] in
//  each arm rather than keeping them in registers.
//============================================================================
#include "pathfind.h"

//----------------------------------------------------------------------------
//  0x6F4A1230 - 1x1.  A diagonal move squeezes between two cells; both of
//  them are the neighbours of the destination *opposite* the direction
//  components, so a unit cannot cut a corner between two blocked cells.
//----------------------------------------------------------------------------
int CPathSearch::TestFootprint1(const SCellPoint* pt, int dir)
{
    int ok = TestCell((unsigned int)pt->m_x, (unsigned int)pt->m_y);

    switch (dir)
    {
    //  The four diagonals are written 9, 3, 6, 12 - the order the shipped
    //  code lays the blocks out in, and the order every one of the four
    //  size classes uses.
    case kPathDirSouth | kPathDirWest:      //  9
        return ok &&
               TestCell((unsigned int)(pt->m_x + 1), (unsigned int)pt->m_y) &&
               TestCell((unsigned int)pt->m_x, (unsigned int)(pt->m_y + 1));

    case kPathDirSouth | kPathDirEast:      //  3
        return ok &&
               TestCell((unsigned int)(pt->m_x - 1), (unsigned int)pt->m_y) &&
               TestCell((unsigned int)pt->m_x, (unsigned int)(pt->m_y + 1));

    case kPathDirEast | kPathDirNorth:      //  6
        return ok &&
               TestCell((unsigned int)(pt->m_x - 1), (unsigned int)pt->m_y) &&
               TestCell((unsigned int)pt->m_x, (unsigned int)(pt->m_y - 1));

    case kPathDirNorth | kPathDirWest:      //  12
        return ok &&
               TestCell((unsigned int)(pt->m_x + 1), (unsigned int)pt->m_y) &&
               TestCell((unsigned int)pt->m_x, (unsigned int)(pt->m_y - 1));
    }
    return ok;
}

//----------------------------------------------------------------------------
//  0x6F4A1350 - 2x2, anchored at its high corner.
//----------------------------------------------------------------------------
int CPathSearch::TestFootprint2(const SCellPoint* pt, int dir)
{
    switch (dir)
    {
    case kPathDirSouth:                     //  1
        return TestCellRun(pt->m_x - 1, pt->m_y - 1, 2);

    case kPathDirEast:                      //  2
        return TestCellColumn(pt->m_x, pt->m_y - 1, 2);

    case kPathDirNorth:                     //  4
        return TestCellRun(pt->m_x - 1, pt->m_y, 2);

    case kPathDirWest:                      //  8
        return TestCellColumn(pt->m_x - 1, pt->m_y - 1, 2);

    case kPathDirSouth | kPathDirWest:      //  9
        return TestCellRun(pt->m_x - 1, pt->m_y - 1, 3) &&
               TestCellColumn(pt->m_x - 1, pt->m_y, 2);

    case kPathDirSouth | kPathDirEast:      //  3
        return TestCellRun(pt->m_x - 2, pt->m_y - 1, 3) &&
               TestCellColumn(pt->m_x, pt->m_y, 2);

    case kPathDirEast | kPathDirNorth:      //  6
        return TestCellRun(pt->m_x - 2, pt->m_y, 3) &&
               TestCellColumn(pt->m_x, pt->m_y - 2, 2);

    case kPathDirNorth | kPathDirWest:      //  12
        return TestCellRun(pt->m_x - 1, pt->m_y, 3) &&
               TestCellColumn(pt->m_x - 1, pt->m_y - 2, 2);
    }
    return TestCellRect(pt->m_x - 1, pt->m_y - 1, 2, 2);
}

//----------------------------------------------------------------------------
//  0x6F4A14D0 - 3x3.
//----------------------------------------------------------------------------
int CPathSearch::TestFootprint3(const SCellPoint* pt, int dir)
{
    switch (dir)
    {
    case kPathDirSouth:                     //  1
        return TestCellRun(pt->m_x - 1, pt->m_y - 1, 3);

    case kPathDirEast:                      //  2
        return TestCellColumn(pt->m_x + 1, pt->m_y - 1, 3);

    case kPathDirNorth:                     //  4
        return TestCellRun(pt->m_x - 1, pt->m_y + 1, 3);

    case kPathDirWest:                      //  8
        return TestCellColumn(pt->m_x - 1, pt->m_y - 1, 3);

    case kPathDirSouth | kPathDirWest:      //  9
        return TestCellRun(pt->m_x - 1, pt->m_y - 1, 4) &&
               TestCellColumn(pt->m_x - 1, pt->m_y, 3);

    case kPathDirSouth | kPathDirEast:      //  3
        return TestCellRun(pt->m_x - 2, pt->m_y - 1, 4) &&
               TestCellColumn(pt->m_x + 1, pt->m_y, 3);

    case kPathDirEast | kPathDirNorth:      //  6
        return TestCellRun(pt->m_x - 2, pt->m_y + 1, 4) &&
               TestCellColumn(pt->m_x + 1, pt->m_y - 2, 3);

    case kPathDirNorth | kPathDirWest:      //  12
        return TestCellRun(pt->m_x - 1, pt->m_y + 1, 4) &&
               TestCellColumn(pt->m_x - 1, pt->m_y - 2, 3);
    }
    return TestCellRect(pt->m_x - 1, pt->m_y - 1, 3, 3);
}

//----------------------------------------------------------------------------
//  0x6F4A1660 - 4x4.
//----------------------------------------------------------------------------
int CPathSearch::TestFootprint4(const SCellPoint* pt, int dir)
{
    switch (dir)
    {
    case kPathDirSouth:                     //  1
        return TestCellRun(pt->m_x - 2, pt->m_y - 2, 4);

    case kPathDirEast:                      //  2
        return TestCellColumn(pt->m_x + 1, pt->m_y - 2, 4);

    case kPathDirNorth:                     //  4
        return TestCellRun(pt->m_x - 2, pt->m_y + 1, 4);

    case kPathDirWest:                      //  8
        return TestCellColumn(pt->m_x - 2, pt->m_y - 2, 4);

    case kPathDirSouth | kPathDirWest:      //  9
        return TestCellRun(pt->m_x - 2, pt->m_y - 2, 5) &&
               TestCellColumn(pt->m_x - 2, pt->m_y - 1, 4);

    case kPathDirSouth | kPathDirEast:      //  3
        return TestCellRun(pt->m_x - 3, pt->m_y - 2, 5) &&
               TestCellColumn(pt->m_x + 1, pt->m_y - 1, 4);

    case kPathDirEast | kPathDirNorth:      //  6
        return TestCellRun(pt->m_x - 3, pt->m_y + 1, 5) &&
               TestCellColumn(pt->m_x + 1, pt->m_y - 3, 4);

    case kPathDirNorth | kPathDirWest:      //  12
        return TestCellRun(pt->m_x - 2, pt->m_y + 1, 5) &&
               TestCellColumn(pt->m_x - 2, pt->m_y - 3, 4);
    }
    return TestCellRect(pt->m_x - 2, pt->m_y - 2, 4, 4);
}
