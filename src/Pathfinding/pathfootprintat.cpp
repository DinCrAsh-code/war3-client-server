//============================================================================
//  0x6F4A3810 - CPathSearch::TestFootprintAt: the same footprint test taken
//  from a grid position rather than a cell.
//
//  Each axis is floored and converted, y before x, which is the right-to-
//  left evaluation of the (x, y) pair - and the reason the shipped code
//  computes the y cell first and still stores it second.
//
//  Its own translation unit: FindFreePoint calls it.
//============================================================================
#include "pathfind.h"

int CPathSearch::TestFootprintAt(const CFloat* pos, const int* mask,
                                 unsigned short size)
{
    SCellPoint pt(CFloatToInt(CFloatFloor(pos[0])),
                  CFloatToInt(CFloatFloor(pos[1])));
    return TestFootprint(&pt, mask, size);
}
