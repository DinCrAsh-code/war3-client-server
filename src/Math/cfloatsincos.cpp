//============================================================================
//  0x6F6EF990 - sine and cosine of a CFloat angle, both from one table
//  lookup, with no FPU involved anywhere.
//
//  How it works, read off the disassembly:
//
//    n = CFloatToInt(angle * kAngleScale)   -- a 20-bit fixed-point turn
//
//  and n is then split three ways:
//
//    bits 0..7    the interpolation fraction, replicated into all four
//                 bytes of a dword (b | b<<8 | b<<16 | b<<24) so that a
//                 64-bit multiply by it and a >>32 is a divide by 255
//                 rather than by 256 - the endpoints then interpolate
//                 exactly, with frac == 0xFF landing on the next entry.
//    bits 8..17   the index into a 1025-entry quarter-wave table
//    bits 18..19  the quadrant.
//
//  The quarter wave is walked forwards on even quadrants and backwards on
//  odd ones (which is what turns a quarter of a sine into a whole one), and
//  bit 1 of the quadrant supplies the sign as an xor/sub mask rather than a
//  branch.  Cosine is the same lookup with the quadrant advanced by one,
//  which is the whole reason the two are computed together: sin and
//  cos(x) = sin(x + pi/2) share the fraction, the index and the table.
//
//  Table entries are 32-bit fixed point with 31 fractional bits, so the
//  interpolated integer is converted with CFloatFromInt and then scaled by
//  2^-31 - not by a multiply, but by subtracting 31 from the exponent field
//  directly (0x0F800000 == 31 << 23).  The subtraction is suppressed when
//  the exponent field is zero, i.e. when the value is zero, which would
//  otherwise underflow into a bogus encoding.
//============================================================================
#include "cmatrix3.h"

//  dword_6FADA910 - turns a CFloat angle in radians into the fixed-point
//  turn the split above expects.
extern const CFloat g_CFloatAngleToTableUnits;

//  dword_6F9784D0 - 1025 entries, a quarter wave inclusive of both ends, so
//  that entry 1024 is reachable as the backward walk's first sample.
extern const int g_CFloatSinTable[1025];

//  The exponent-field bias of 31, subtracted to scale by 2^-31.
static const int kCFloatExp31 = 31 * kCFloatExpOne;

#define SAMPLE_QUARTER_WAVE(quadrant, out)                                    \
    {                                                                         \
        int value_;                                                           \
        if (((quadrant) & 1) != 0)                                            \
        {                                                                     \
            int hi_ = g_CFloatSinTable[1024 - index];                         \
            int lo_ = g_CFloatSinTable[1023 - index];                         \
            value_ = hi_ - (int)(((unsigned __int64)(unsigned int)(hi_ - lo_) \
                                  * frac) >> 32);                             \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            int lo_ = g_CFloatSinTable[index];                                \
            int hi_ = g_CFloatSinTable[index + 1];                            \
            value_ = lo_ + (int)(((unsigned __int64)(unsigned int)(hi_ - lo_) \
                                  * frac) >> 32);                             \
        }                                                                     \
        int sign_ = ((quadrant) << 30) >> 31;                                 \
        CFloat sample_ = CFloatFromInt((value_ ^ sign_) - sign_);             \
        int bits_ = sample_.m_bits;                                           \
        int scale_ = ((bits_ & kCFloatExpMask) != 0) ? kCFloatExp31 : 0;      \
        (out)->m_bits = bits_ - scale_;                                       \
    }

void __fastcall CFloatSinCos(const CFloat& angle, CFloat* outSin,
                             CFloat* outCos)
{
    CFloat scaled = angle * g_CFloatAngleToTableUnits;
    int n = CFloatToInt(scaled);

    unsigned int frac = (unsigned int)(unsigned char)n;
    frac = frac | (frac << 8);
    frac = frac | (frac << 16);

    int index = (n >> 8) & 0x3FF;
    int quadrant = n >> 18;

    //  Spelled out twice through a macro rather than called twice through a
    //  helper: the shipped code has both copies inline, and a static helper
    //  returning CFloat by value is over what /Ob2 will inline here - it
    //  stayed a real call and the function came out 46 instructions against
    //  the shipped 127.  A macro is the one spelling that reproduces it
    //  without a __forceinline the original build had no way to write.
    SAMPLE_QUARTER_WAVE(quadrant, outSin)
    SAMPLE_QUARTER_WAVE(quadrant + 1, outCos)
}
