//============================================================================
//  0x6F4A0980 - CFloatFloorSplit(), run over both axes of a CGridVec2, each
//  axis's fractional remainder then clamped against the matching radius
//  component before being added back onto the floored integer part:
//
//      frac = value - floor(value)
//      frac = (frac < radius.x) ? radius.x
//           : (frac > radius.y) ? radius.y
//           : frac
//      value = floor(value) + frac
//
//  This is the "arguments in eax and esi" whole-program calling convention
//  documented in docs/msvc-vc8-idioms.md: the real callee (sub_6F4A3E80's
//  loop) sets up `pos` in edi and `radius` in eax with no explicit load of
//  either into this function's own body - both registers are simply
//  whatever the caller already had live.  A single translation unit cannot
//  reproduce that; this is written as an ordinary __fastcall(ecx=pos,
//  edx=radius) instead, matching the technique's caller-side cost (an extra
//  push/reload per argument) rather than the callee's own frame - see
//  "Arguments in eax and esi: the shipped build used whole-program codegen".
//============================================================================
#include "game.h"

void __fastcall CFloatFloorSplit(const CFloat* value, CFloat* remainderOut,
                                 CFloat* intPartOut);

void __fastcall FloorSplitClampVec(CGridVec2* pos, const CGridVec2* radius)
{
    CFloat fracX, intX;
    CFloatFloorSplit(&pos->m_x, &fracX, &intX);

    CFloat fracY, intY;
    CFloatFloorSplit(&pos->m_y, &fracY, &intY);

    const CFloat* px;
    if (*(const float*)&fracX.m_bits < *(const float*)&radius->m_x.m_bits)
    {
        px = &radius->m_x;
    }
    else
    {
        px = &radius->m_y;
        if (!(*(const float*)&fracX.m_bits > *(const float*)&radius->m_y.m_bits))
            px = &fracX;
    }
    fracX = *px;
    pos->m_x = intX + fracX;

    if (*(const float*)&fracY.m_bits < *(const float*)&radius->m_x.m_bits)
    {
        px = &radius->m_x;
    }
    else
    {
        px = &radius->m_y;
        if (!(*(const float*)&fracY.m_bits > *(const float*)&radius->m_y.m_bits))
            px = &fracY;
    }
    fracY = *px;
    pos->m_y = intY + fracY;
}
