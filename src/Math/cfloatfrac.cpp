//============================================================================
//  CFloat's fractional-part helper and a generic integer-power leaf, both
//  reached only from SCFloatFormatter::FormatDecimal (0x6F6F0360, Storm/
//  textsinkscratch.cpp - a hand-rolled dtoa left as a naked redirect, out of
//  that file's own scope) via its own callee sub_6F6EFE50 - CItem vtable
//  BFS batch 8 of 8, docs/targets/CItem-callee-batch8.md.
//============================================================================
#include "CFloat.h"

//----------------------------------------------------------------------------
//  0x6F6EECE0 - binary exponentiation, `__fastcall(ecx=base, edx=exponent)`,
//  no stack arguments, `retn 0`.  Used by the decimal formatter to build a
//  power-of-ten scale factor.
//----------------------------------------------------------------------------
int __fastcall IntPow(int base, unsigned int exponent)
{
    int result = 1;

    if (exponent != 0)
    {
        do
        {
            if (exponent & 1)
                result *= base;

            int b = base;
            base = b * b;

            exponent >>= 1;
        } while (exponent != 0);
    }

    return result;
}

//----------------------------------------------------------------------------
//  0x6F6EF2D0 - fractional part: `b - trunc(b)`, `__fastcall(ecx=hidden
//  return buffer, edx=&b)`, `retn 0`.
//
//  |b| < 1.0 (unbiased exponent < 0): the whole value is its own fractional
//  part, returned verbatim with no arithmetic.
//  |b| >= 2^23 (unbiased exponent >= 23): every mantissa bit is already an
//  integer bit, so the fractional part is exactly zero.
//  Otherwise trunc(b) is built by masking off every mantissa bit below the
//  unbiased exponent's own bit position - kept: sign, exponent, and the top
//  `unbiased` mantissa bits - with one arithmetic right shift of a
//  top-bit-set pattern (`(int)0x80000000 >> (unbiased+8)`) rather than a
//  mask table, then operator-() (0x6F6EEEF0, softfloat.cpp) is called on the
//  original value and that truncated one.
//----------------------------------------------------------------------------
CFloat __fastcall CFloatFrac(const CFloat& b)
{
    int bits = b.m_bits;
    int unbiased = ((bits >> 23) & 0xFF) - 0x7F;

    if (unbiased < 0)
        return b;

    if (unbiased >= 23)
        return CFloat(0);

    int mask = (int)0x80000000 >> (unbiased + 8);
    return b - CFloat(mask & bits);
}
