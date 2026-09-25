#include "quadrant.h"

//----------------------------------------------------------------------------
//  0x6F00C3F0
//----------------------------------------------------------------------------
int __fastcall NearlyEqual(const CFloat& a, const CFloat& b)
{
    int magnitude = (a - b).m_bits & 0x7FFFFFFF;
    //  The `(bool)` is load-bearing: the shipped code materialises 0/1 in al
    //  and widens it with `movzx`, which is what a bool-to-int conversion
    //  does and what a bare comparison result does not.
    return (bool)(int)(*(float*)&magnitude < g_quadrantEpsilon);
}

//----------------------------------------------------------------------------
//  0x6F00CDE0
//----------------------------------------------------------------------------
int __stdcall ClassifyQuadrant(CFloat value)
{
    if (NearlyEqual(value, g_CFloatZero))
        return 0;
    if (NearlyEqual(value, g_quadrantEast))
        return 1;
    if (NearlyEqual(value, g_quadrantNorth))
        return 2;

    NearlyEqual(value, g_quadrantWest);
    return 3;
}
