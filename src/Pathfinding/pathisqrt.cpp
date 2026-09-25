//============================================================================
//  0x6F4BABB0 - integer square root by Newton's method, the one both A*
//  instances take every edge cost and every heuristic through.
//
//  Not the restoring-square-root loop in pathsqrt.cpp (0x6F6EED10): that one
//  works a bit at a time on a normalised mantissa for CFloatSqrt, this one
//  is a plain 32-bit unsigned isqrt.
//
//  The initial estimate is piecewise linear in three magnitude buckets, and
//  every one of the three deliberately over-estimates - Newton's iteration
//  for a square root converges monotonically from above, so an estimate that
//  came in low would approach the answer from the wrong side.  The three
//  divisors are recovered from the shipped magic-number multiplies:
//
//      0xAAAAAAAB, mulhi >> 3   -> / 12
//      0x51EB851F, mulhi >> 6   -> / 200
//      0x39ACC69D with the add-back form, >> 15 total -> / 26743
//
//  The loop halves the gap each round and stops when the *previous* estimate
//  and the quotient have converged to within one, which is one iteration
//  later than testing the new estimate would be and is why the difference is
//  kept in its own variable rather than recomputed.
//
//  Takes its argument in ecx and returns in eax - one __fastcall register
//  argument, no stack.
//============================================================================
#include "game.h"

unsigned int __fastcall IntegerSqrtNewton(unsigned int value)
{
    int estimate;

    if (value <= 0xFF)
        estimate = value / 12 + 1;
    else if (value <= 0xFFFF)
        estimate = value / 200 + 21;
    else
        estimate = value / 26743 + 444;

    int gap;
    do
    {
        int quotient = (int)(value / (unsigned int)estimate);
        gap = (estimate - quotient) / 2;
        estimate = (estimate + quotient) / 2;
    }
    while (gap != 0);

    return estimate;
}
