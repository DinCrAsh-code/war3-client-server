//============================================================================
//  0x6F6EFB00 - CFloatAcos: the deterministic arc cosine.
//
//  Two piecewise-linear table lookups, picked by how close |x| is to 1.
//
//  For |x| <= ~0.994 (bit pattern 0x3F7E8000) the input is scaled by 2^30
//  by adding 0x0F000000 to its exponent field, converted to a fixed-point
//  integer, saturated to +/-0x3FFFFFFF, and split into a 10-bit table index
//  (the top bits) and a 20-bit fraction (rotated up into the high word so
//  the interpolation can be a single 32x32 high multiply).  The table holds
//  the quarter turn in Q2.30 - 0x6487ED51 is pi/2 in that format, and the
//  negative half of the domain is read out of the same table backwards from
//  its far end, subtracted from that constant.
//
//  For |x| above that the curve is too steep to sample evenly, so the
//  second table is indexed by the *leading zero count* of (1.0 - |x|)
//  taken on the bit patterns - a logarithmic index that puts more entries
//  where acos changes fastest - with the next four bits of the normalised
//  remainder choosing between the sixteen entries in that octave.
//
//  Both branches finish the same way: convert the fixed-point result back
//  to a CFloat and scale it down by adjusting the exponent field directly
//  (2^29 for the first branch, 2^34 for the second), skipping the
//  adjustment when the result came back as an exact zero, whose exponent
//  field must stay zero.  A negative input then takes the identity
//  acos(-x) = pi - acos(x).
//============================================================================
#include "CFloat.h"

//  dword_6F9794D8 / unk_6F97A4D8 - one table, addressed from both ends.
//  The two spellings are two symbols at two addresses (0x1000 apart, so
//  1024 entries between them) rather than one array indexed two ways: the
//  shipped code materialises the far end as its own address constant, and
//  an array-plus-offset expression here would not reproduce that operand.
extern const int g_acosCoarseTable[];      // 0x6F9794D8
extern const int g_acosCoarseTableNext[];  // 0x6F9794DC - &g_acosCoarseTable[1]
extern const int g_acosCoarseTableTop[];   // 0x6F97A4D8 - &g_acosCoarseTable[1024]
extern const int g_acosCoarseTablePrev[];  // 0x6F97A4D4 - the entry below it

//  dword_6F97A4E0 - the near-one octave table.
extern const int g_acosNearOneTable[];     // 0x6F97A4E0
extern const int g_acosNearOneTableNext[]; // 0x6F97A4E4 - &g_acosNearOneTable[1]

//  dword_6FAAE600 - encoded pi.  Spelled `g_quadrantNorth` because that is
//  the name funcmap.py's DATA table binds to this address (quadrant.h
//  declares it, quadrant.cpp uses it).  pathmove.h also declares a
//  `g_CFloatPi` at the same address; nothing references it, and referencing
//  it here would have produced a second symbol with no address behind it.
extern const CFloat g_quadrantNorth;       // 0x6FAAE600

//  pi/2 in Q2.30, the format both tables are stored in.
const int kAcosQuarterTurnQ30 = 0x6487ED51;

CFloat __fastcall CFloatAcos(const CFloat& x)
{
    //  kCFloatNoInit, not the zeroing default constructor: this is the
    //  named return value, so the constructor's store lands on the caller's
    //  own buffer and the shipped code does not make it (CFloat.h).
    CFloat result(kCFloatNoInit);
    result.m_bits = x.m_bits & 0x7FFFFFFF;

    if ((unsigned int)result.m_bits <= 0x3F7E8000u)
    {
        result.m_bits = x.m_bits + 0x0F000000;

        //  The saturating clamp to +/-0x3FFFFFFF, spelled the way the
        //  shipped code computes it: the sign is taken from the *biased*
        //  value, not from `fixed` itself, which is what puts the `sar` on
        //  the already-computed range-test operand instead of on a second
        //  copy of the input.
        int fixed = CFloatToInt(result);
        int biased = fixed + 0x3FFFFFFF;
        if ((unsigned int)biased > 0x7FFFFFFEu)
            fixed = (~(biased >> 31) & 0x7FFFFFFE) - 0x3FFFFFFF;

        //  `shifted | (shifted >> 20)`, not a rotate of `fixed`: the low
        //  twelve bits of the shifted value are zero, so folding its own
        //  top twelve bits back down into them smears the fraction out to
        //  full width - which is what makes the interpolation land exactly
        //  on the next table entry at the top of a step.  Written as
        //  `(fixed << 12) | (fixed >> 20)` this compiler recognises a
        //  rotate and emits one `rol`, which is a different value.
        unsigned int shifted  = (unsigned int)fixed << 12;
        unsigned int fraction = shifted | (shifted >> 20);
        int index = (fixed >> 20) & 0x3FF;

        int base;
        int slope;
        if (fixed >= 0)
        {
            base  = g_acosCoarseTable[index];
            slope = base - g_acosCoarseTableNext[index];
        }
        else
        {
            //  Two symbols, one per entry, rather than one symbol indexed
            //  twice: the shipped code materialises both far-end addresses
            //  as their own constants and subtracts the same `index*4` from
            //  each.  Indexed off one symbol this compiler computes the
            //  address once and reads `[eax]` / `[eax-4]`, which is the
            //  same two loads spelled with one fewer address constant.
            int high = g_acosCoarseTableTop[-index];
            slope = g_acosCoarseTablePrev[-index] - high;
            base  = kAcosQuarterTurnQ30 - high;
        }

        int value = base - (int)(((unsigned __int64)(unsigned int)slope * fraction) >> 32);

        int bits = CFloatFromInt(value).m_bits;
        bits -= (bits & 0x7F800000) != 0 ? 0x0E800000 : 0;
        result.m_bits = bits;
        return result;
    }

    int magnitude = result.m_bits;
    if ((unsigned int)magnitude > 0x3F800000u)
        magnitude = ~(magnitude >> 31) & 0x3F800000;

    int gap = 0x3F800000 - magnitude;
    int shift = CountLeadingZeros(gap);

    //  An if/else, not a zeroed local the `if` overwrites: the shipped
    //  code materialises the zero in its *second* branch
    //  (docs/msvc-vc8-idioms.md, "An explicit `xor eax,eax` says which
    //  branch was written second").
    unsigned int remainder;
    if ((unsigned int)shift < 32u)
        remainder = (unsigned int)~gap << shift;
    else
        remainder = 0;

    int octave = (shift * 8 - 0x78) | (int)(remainder >> 28);

    int nearBase  = g_acosNearOneTable[octave];
    int nearSlope = nearBase - g_acosNearOneTableNext[octave];
    unsigned int nearFraction = remainder << 4;

    int nearValue = nearBase -
        (int)(((unsigned __int64)(unsigned int)nearSlope * nearFraction) >> 32);

    int nearBits = CFloatFromInt(nearValue).m_bits;
    nearBits -= (nearBits & 0x7F800000) != 0 ? 0x11000000 : 0;
    result.m_bits = nearBits;

    if (x.m_bits & 0x80000000)
    {
        result = g_quadrantNorth - result;
        return result;
    }

    return result;
}
