//============================================================================
//  0x6F6EF170, 0x6F47D5D0, 0x6F47D560 - the box helper module, plus
//  0x6F47BA20 (CCellGrid::FloatBoxToCellBox), which lives at the same
//  addresses and is a member of the grid even though this build's body never
//  reads `this`.
//
//  All four return their result by value.  That is not cosmetic: VC8 passes
//  the hidden return-buffer pointer where a __fastcall's first argument would
//  go (ecx for the free functions, the first stack slot for the member), and
//  returns it in eax - which is exactly the `retn 4` / `retn 8` shapes and
//  the eax-is-the-out-pointer convention the dump has.
//============================================================================
#include "boxmath.h"
#include "spatialgrid.h"

//----------------------------------------------------------------------------
//  0x6F6EF170 - floor(x), on the encoded bit pattern rather than the FPU.
//
//  Three cases, in the order the shipped code takes them:
//
//    exponent < 0  (|x| < 1)   ->  -1.0 for a negative non-zero x, else +0.0
//    exponent >= 23            ->  x is already integral, pass it through
//    otherwise                 ->  clear the fractional mantissa bits, and
//                                  for a negative x round the magnitude *up*
//                                  first (that is what floor means below
//                                  zero), letting the carry out of bit 23
//                                  bump the exponent.
//
//  The two sign tests are written the way the shipped code computes them -
//  `(x+x) & (x>>31)` for "negative and non-zero", `(x+x) & ~(x>>31)` for
//  "positive and non-zero" - rather than as `x < 0 && ...`: the arithmetic
//  shift produces an all-ones mask and the `test` of the two is a single
//  branch, which is the whole point of the idiom and what the disassembly
//  has.  selftest.cpp checks the result against the hardware floorf() over a
//  wide sweep of exponents and both signs.
//----------------------------------------------------------------------------
CFloat __fastcall CFloatFloor(const CFloat& value)
{
    int bits = value.m_bits;
    int exponent = (int)(((unsigned int)bits >> 23) & 0xFF) - 0x7F;

    if (exponent < 0)
        return ((bits >> 31) & (bits + bits)) ? g_CFloatMinusOne : g_CFloatZero;

    if (exponent < 23)
    {
        //  Bits [31 .. 23-exponent] - sign, exponent and the integral part of
        //  the mantissa.  An arithmetic shift of the sign bit builds it in one
        //  step.
        int keep = (int)0x80000000 >> (exponent + 8);

        if (~(bits >> 31) & (bits + bits))
            return CFloat(bits & keep);

        int mantissa = (bits & kCFloatMantMask) + ~keep;
        mantissa &= keep;
        //  A carry out of the mantissa lands in bit 23 and is the exponent
        //  increment; the sign is put back by hand because `keep` cleared it.
        exponent += (int)((unsigned int)mantissa >> 23);
        exponent += 0x7F;
        return CFloat((exponent << 23) | (mantissa & kCFloatMantMask) |
                      kCFloatSignMask);
    }

    return CFloat(bits);
}

//----------------------------------------------------------------------------
//  0x6F47D5D0 - component-wise multiply.  One constructor call: the four
//  products are argument expressions, so VC8 evaluates them right to left
//  (x1, y1, x0, y0 - the order the four CFloat::operator* calls come in) and
//  stores them in member order.
//----------------------------------------------------------------------------
SCellBoxF __fastcall MulCellBoxF(const SCellBoxF& a, const SCellBoxF& b)
{
    return SCellBoxF(a.m_y0 * b.m_y0, a.m_x0 * b.m_x0,
                     a.m_y1 * b.m_y1, a.m_x1 * b.m_x1);
}

//----------------------------------------------------------------------------
//  0x6F47D560 - rectangle intersection.  Every field is computed before any
//  is stored, and the store order (x1, x0, y0, y1) is not the compute order
//  (x1, y1, x0, y0).
//----------------------------------------------------------------------------
SCellBox __fastcall IntersectCellBox(const SCellBox& a, const SCellBox& b)
{
    int x1 = (a.m_x1 < b.m_x1) ? a.m_x1 : b.m_x1;
    int y1 = (a.m_y1 < b.m_y1) ? a.m_y1 : b.m_y1;
    int x0 = (a.m_x0 > b.m_x0) ? a.m_x0 : b.m_x0;
    int y0 = (a.m_y0 > b.m_y0) ? a.m_y0 : b.m_y0;

    SCellBox r;
    r.m_x1 = x1;
    r.m_x0 = x0;
    r.m_y0 = y0;
    r.m_y1 = y1;
    return r;
}

//----------------------------------------------------------------------------
//  0x6F47BA20 - turn a rectangle already scaled into cell units into the
//  half-open cell rectangle that covers it: floor() each edge, convert to
//  int, and add one to the two max edges (and only those).
//
//  `this` is the grid - every caller loads it - but nothing in the body uses
//  it, so the shipped code's `retn 8` is a member call whose ecx is dead and
//  whose two stack slots are the hidden return buffer and the argument.
//  Declaring it a member returning by value is what reproduces that.
//----------------------------------------------------------------------------
SCellBox CCellGrid::FloatBoxToCellBox(const SCellBoxF& box)
{
    //  Four named temporaries, not four nested calls: the shipped code gives
    //  each floor() result its own stack slot instead of reusing one.
    CFloat fx1 = CFloatFloor(box.m_x1);
    int x1 = CFloatToInt(fx1);
    CFloat fy1 = CFloatFloor(box.m_y1);
    int y1 = CFloatToInt(fy1);
    CFloat fx0 = CFloatFloor(box.m_x0);
    int x0 = CFloatToInt(fx0);
    CFloat fy0 = CFloatFloor(box.m_y0);
    int y0 = CFloatToInt(fy0);

    y1 = y1 + 1;
    x1 = x1 + 1;

    SCellBox r;
    r.m_x1 = x1;
    r.m_x0 = x0;
    r.m_y0 = y0;
    r.m_y1 = y1;
    return r;
}
