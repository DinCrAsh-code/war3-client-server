//============================================================================
//  0x6F6EF290 - mask a CFloat down to its integer part in place (bit
//  pattern, not value): magnitude below 1 flushes to exactly 0, magnitude
//  at or above 2^23 (already an integer, no fractional mantissa bits to
//  drop) passes through unchanged, and everything between has its
//  fractional mantissa bits masked off by an arithmetic right shift of a
//  single set sign bit - the same "shift 0x80000000 by exponent+8" trick
//  CFloatFloor (softfloat.cpp) uses for its own keep-mask, just without
//  CFloatFloor's rounding-up-the-magnitude or sign handling.  Nothing in
//  this call tree names what calls this a "floor" or a "trunc"; it is
//  named for what the bit pattern does.
//
//  CRASH ROOT CAUSE (found via live debugger, MoveForward_run1): the target
//  begins with `mov eax, ecx` and never touches eax again on the way to any
//  of its three `ret`s - every path returns with eax still holding the
//  incoming `dest` pointer (the shift computation moves into ecx instead so
//  eax can stay untouched).  The prior session declared this `void`, so the
//  compiler had no reason to preserve `dest` in eax and used it as scratch
//  for the shift math instead - eax at return held leftover shift-exponent
//  arithmetic, not the pointer.  That is invisible to the per-function
//  instruction diff (which only checks that operands are used consistently
//  within the body, not what a caller reads out of eax afterwards) but this
//  call tree's own caller, `sub_6F4A07C0`, does
//  `call sub_6F6EF290 / mov eax, [eax]` immediately after - it dereferences
//  the return value as the pointer it just wrote through.  With the old
//  `void` signature that dereferenced whatever garbage was left in eax by
//  the shift computation, an out-of-bounds read.  Returning `dest` (the
//  same "hand back the pointer you were given" shape `CAgentPtr::AssignChecked`
//  already uses in agent.cpp) restores it.
//============================================================================
#include "CFloat.h"

CFloat* __fastcall CFloatTruncateFrac(CFloat* dest, const CFloat* src)
{
    int* out = &dest->m_bits;
    int bits = src->m_bits;
    int shift = (int)(((unsigned int)bits >> 23) & 0xFF) - 0x7F;

    if (shift < 0)
    {
        *out = 0;
        return dest;
    }
    if (shift >= 0x17)
    {
        *out = bits;
        return dest;
    }

    int keepMask = (int)0x80000000 >> (shift + 8);
    *out = keepMask & bits;
    return dest;
}
