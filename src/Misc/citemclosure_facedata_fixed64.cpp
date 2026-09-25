//============================================================================
//  FACEDATA's own extended-precision (64-bit-as-two-ints) fixed helpers -
//  CItem vtable slots 32/104 closure, batch K, depth 12-25+.  Own
//  translation unit: Misc/citemclosure_facedata_metrics.cpp's own FixedDiv
//  calls Fixed64Add and Fixed64Divide for real (the shipped stream shows
//  both as `call`s, not inlined), and /Ob2 will happily inline either one
//  into a same-TU caller this small - measured, and it costs both callers'
//  own scores when it does.  See citemclosure_facedata_metrics.cpp's own
//  header for how "FACEDATA" is ground truth rather than a guess.
//============================================================================
#include "storm.h"

//----------------------------------------------------------------------------
//  A 64-bit fixed value carried as two plain ints, the same way the eight
//  bytes at [ecx]/[ecx+4] and [edx]/[edx+4] are read below: an unsigned
//  low half and a signed high half, added with a manual carry rather than
//  through the hardware's own 64-bit add (this build's calling convention
//  never puts the two 32-bit halves in one register pair to begin with).
//----------------------------------------------------------------------------
struct FACEDATA_Fixed64
{
    unsigned int lo;   // 0x00
    int          hi;   // 0x04
};

//----------------------------------------------------------------------------
//  0x6F7C9410 - FACEDATA::Fixed64Add: *out = *a + *b, carrying the low
//  half's unsigned overflow into the high half by hand (`cmp`/`sbb`/`neg`
//  is exactly the textbook carry-out-of-an-add idiom).
//----------------------------------------------------------------------------
void __fastcall FACEDATA_Fixed64Add(FACEDATA_Fixed64* a, FACEDATA_Fixed64* b,
                                     FACEDATA_Fixed64* out)
{
    unsigned int lo = b->lo + a->lo;
    int carry = (lo < a->lo) ? 1 : 0;
    int hi = carry + a->hi + b->hi;

    out->lo = lo;
    out->hi = hi;
}

//----------------------------------------------------------------------------
//  0x6F7C94B0 - FACEDATA::Fixed64Divide: divide the 64-bit signed value at
//  `v` (unsigned low half at +0, signed high half at +4) by the plain int
//  `divisor`, returning a 32-bit signed quotient.  A negative high half is
//  negated in place first (two's-complement of the 64-bit pair, done
//  through memory the way Fixed64Add's own carry-out is); the common case
//  (high half zero after that) uses the hardware `div` directly, an
//  out-of-range case clamps to 0x7FFFFFFF/-0x7FFFFFFF, and everything else
//  falls through to a 32-iteration restoring long division.  Transcribed
//  close to the shipped control flow (register-named locals) rather than
//  reshaped: the loop's carry-in/carry-out through `ecx`/`eax`/`edx` is the
//  textbook long-division idiom and reshaping it risks losing it, the way
//  CLAUDE.md's own note on the four spin-reload functions warns against.
//----------------------------------------------------------------------------
int __fastcall FACEDATA_Fixed64Divide(FACEDATA_Fixed64* v, int divisor)
{
    int ebx = v->hi;
    int esi = divisor;

    if (ebx < 0)
    {
        unsigned int lo = (unsigned int)(-(int)v->lo);
        v->lo = lo;
        int carry = (lo == 0) ? 1 : 0;
        v->hi = carry + ~ebx;
    }

    ebx = ebx ^ esi;   // combined sign, replacing the plain high half
    if (esi < 0)
        esi = -esi;

    int eax = v->hi;
    if (eax == 0)
    {
        if (esi > 0)
            eax = (int)((unsigned int)v->lo / (unsigned int)esi);
        else
            eax = 0x7FFFFFFF;

        return (ebx >= 0) ? eax : -eax;
    }

    if ((unsigned int)eax >= (unsigned int)esi)
    {
        unsigned int flag = (ebx < 0) ? 1u : 0u;
        return (int)(2u * flag + 0x7FFFFFFFu);
    }

    {
        int ecx = (int)v->lo;
        int edx = 0;
        int edi = 0x20;
        do
        {
            int ebp = (int)((unsigned int)ecx >> 31);
            eax = eax + eax;
            eax = eax | ebp;
            edx = edx + edx;
            if ((unsigned int)eax >= (unsigned int)esi)
            {
                eax = eax - esi;
                edx = edx | 1;
            }
            ecx = ecx + ecx;
            edi = edi - 1;
        } while (edi != 0);

        return (ebx >= 0) ? edx : -edx;
    }
}
