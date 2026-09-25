//============================================================================
//  0x6F4A07C0 - split a CFloat into its floor and the fractional remainder:
//  *intPartOut is always an integer, *remainderOut = *value - *intPartOut.
//
//  CFloatTruncateFrac() truncates toward zero, so for a negative,
//  non-integer value it overshoots the floor by exactly 1.0
//  (g_slopeThreshold1, the same encoded-1.0 constant ClassifySlopeAxis's own
//  threshold - see pathtracegrid.cpp) - subtracting it is what the second
//  branch below corrects for.  The correction is skipped for exactly
//  0x80000000 ("negative zero"): doubling that bit pattern gives 0, same as
//  doubling any non-negative value, and the shipped code folds "negative
//  AND not that one exception" into a single sign-mask-and-test rather than
//  two branches (`sar ecx,1Fh` builds an all-ones/all-zeros mask from the
//  sign, `lea edx,[eax+eax]` is `bits+bits`, `test edx,ecx` is
//  `(bits<0) ? (bits+bits) : 0`, nonzero exactly when bits is negative and
//  not 0x80000000) - see docs/msvc-vc8-idioms.md.
//============================================================================
#include "CFloat.h"

CFloat* __fastcall CFloatTruncateFrac(CFloat* dest, const CFloat* src);
extern const CFloat g_slopeThreshold1;   // dword_6FAAE4F4

void __fastcall CFloatFloorSplit(const CFloat* value, CFloat* remainderOut,
                                 CFloat* intPartOut)
{
    //  The truncated value is built in its own local first and copied into
    //  *intPartOut afterwards - two instructions the direct-write shape
    //  `CFloatTruncateFrac(intPartOut, value);` does not produce.
    CFloat trunc;
    CFloatTruncateFrac(&trunc, value);
    *intPartOut = trunc;

    //  `(bits>>31) & (bits+bits)` is nonzero exactly when bits is negative
    //  and not 0x80000000 ("negative zero") - see the header comment.  The
    //  correction, when it applies, is a three-way pointer select (the same
    //  shape ClampToGameBounds uses): a named local holds the corrected
    //  value, and one shared store at the end goes through whichever
    //  address was chosen.
    int bits = value->m_bits;
    const CFloat* px = intPartOut;
    CFloat corrected;
    if (((bits >> 31) & (bits + bits)) != 0)
    {
        corrected = *intPartOut - g_slopeThreshold1;
        px = &corrected;
    }
    *intPartOut = *px;

    *remainderOut = *value - *intPartOut;
}
