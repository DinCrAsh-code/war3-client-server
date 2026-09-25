//============================================================================
//  0x6F3E0B50 - snap a position onto a footprint's cell grid for one facing.
//
//  The position is clamped into the map, truncated to a multiple of 64 grid
//  units (toward zero - it is a signed `% 64`, not a mask), optionally
//  stepped half a cell along each axis, converted back to CFloat and finally
//  offset by half a cell again on whichever axes the shape asks for.  Which
//  of the shape's two flag words drives which axis depends on the facing:
//  the two "straight" facings (0 and 2) take them one way round, the two
//  diagonal ones the other.
//
//  Both half-cell steps follow the sign of the (clamped) coordinate, so a
//  footprint on the negative side of the origin is snapped away from it by
//  the same amount as one on the positive side.
//============================================================================
#include "footprinttype.h"
#include "gamebounds.h"

//  Half a grid cell, and the offset every non-centred axis gets.
extern const CFloat g_halfCellOffset;    // dword_6FAB68A8

void CFootprintShape::ApplyDirection(CFloat* x, CFloat* y, int facing)
{
    CGridVec2 p;
    p.m_x = *x;
    p.m_y = *y;

    //  All four selections are written out rather than hoisted into a
    //  `primary`/`secondary` pair: the shipped code re-reads the flag word
    //  inside each of the four branches, which is what four separate
    //  conditional expressions produce and what two named locals does not.
    int straight = (facing == 0 || facing == 2);
    int centreX = (int)(~(straight ? m_flags8 : m_flagsC)) & 1;
    int centreY = (int)(~(straight ? m_flagsC : m_flags8)) & 1;
    int stepX = (int)((straight ? m_flags8 : m_flagsC) >> 1) & 1;
    int stepY = (int)((straight ? m_flagsC : m_flags8) >> 1) & 1;

    ClampToGameBounds(&p);

    //  The two sign flags are widened bools, not ints: the shipped code
    //  materialises 0/1 in `al` from the branch and then `movzx`es it into
    //  the int it stores, which is what a bool-to-int conversion does and
    //  what a plain `int` comparison result does not.
    int negX = (bool)(int)(*(const float*)&p.m_x < *(const float*)&g_CFloatZero);
    int negY = (bool)(int)(*(const float*)&p.m_y < *(const float*)&g_CFloatZero);

    int ix = CFloatToInt(p.m_x);
    int iy = CFloatToInt(p.m_y);
    ix -= ix % 64;
    iy -= iy % 64;

    if (negX)
        stepX = -stepX;
    ix += stepX << 5;
    if (negY)
        stepY = -stepY;
    iy += stepY << 5;

    p.m_x = CFloatFromInt(ix);
    p.m_y = CFloatFromInt(iy);

    p.m_x = p.m_x + (centreX
                         ? g_CFloatZero
                         : (negX ? CFloat(g_halfCellOffset.m_bits ^
                                          kCFloatSignMask)
                                 : g_halfCellOffset));
    p.m_y = p.m_y + (centreY
                         ? g_CFloatZero
                         : (negY ? CFloat(g_halfCellOffset.m_bits ^
                                          kCFloatSignMask)
                                 : g_halfCellOffset));

    *x = p.m_x;
    *y = p.m_y;
}
