//============================================================================
//  0x6F6EED10 / 0x6F6EF750 - CFloat's own square root.
//
//  IntegerSqrt is the restoring square-root loop the encoded-float version
//  runs on a normalised 32-bit mantissa (16 iterations, one bit of the
//  result per iteration); its real input arrives in edx, not ecx - the
//  first fastcall register is dead on entry (docs/msvc-vc8-idioms.md,
//  "Arguments in eax and esi"), so `unused` stands in for it here.
//  CFloatSqrt is the float wrapper: normalise the mantissa to 32 bits,
//  IntegerSqrt it, then apply a magic-constant correction
//  (0xB504/0xB505 approximate sqrt(2) in Q16 fixed point - the standard fix
//  for a fixed-point sqrt's one-bit exponent-parity error) before
//  rebuilding the encoded result through CFloat::operator*.  Transcribed as
//  __asm, the same technique CountLeadingZeros/CFloatToInt use
//  (softfloat.cpp): the bit-level shape is the whole content.
//============================================================================
#include "CFloat.h"

#pragma warning(disable : 4035)

//  __declspec(naked), not a plain __asm body: as an ordinary function the
//  compiler wraps the block in its own `push esi` / `push edi` register
//  save (it sees both clobbered) *on top of* the source's own `push edi`,
//  which is one push and one pop the shipped function has not got.  The
//  prologue is part of the transcription, so it is written out here - which
//  is what takes this from 25 of 25 with a 27-instruction stream to an
//  exact match.  Everything between the labels is unchanged.
__declspec(naked) unsigned int __fastcall IntegerSqrt(unsigned int /*unused*/,
                                                      unsigned int /*value*/)
{
    __asm
    {
        push    esi
        xor     ecx, ecx
        push    edi
        xor     eax, eax
        lea     esi, [ecx+10h]
        lea     esp, [esp+0]                // the shipped loop-alignment pad
    loop_top:
        mov     edi, edx
        add     ecx, ecx
        shr     edi, 1Eh
        add     ecx, ecx
        add     edx, edx
        or      ecx, edi
        add     eax, eax
        add     edx, edx
        cmp     eax, ecx
        jnb     skip
        or      edi, 0FFFFFFFFh
        sub     edi, eax
        add     ecx, edi
        add     eax, 2
    skip:
        sub     esi, 1
        jnz     loop_top
        pop     edi
        shr     eax, 1
        pop     esi
        retn
    }
}

CFloat __fastcall CFloatSqrt(const CFloat& value)
{
    int bits = value.m_bits;
    int nonNeg = ~(bits >> 31);
    if (((bits + bits) & nonNeg) == 0)
        return CFloat(0);

    int mant = bits & kCFloatMantMask;
    unsigned int normMant = (unsigned int)(mant | 0xFF800000) << 8 |
                            (unsigned int)mant >> 15;
    int exp = (bits >> 23) & 0xFF;
    exp -= 0x7F;

    unsigned int root = IntegerSqrt(0, normMant);
    root -= 0xB504;
    root *= 0xB505;
    root >>= 8;
    root |= 0x3F800000;
    CFloat halfRootBase(root);

    //  exp's own parity picks which of the two fixed constants below the
    //  correction multiplies by - the shipped code computes both the
    //  biased exponent and this sign-extended parity bit from the same
    //  value rather than a modulo, which is what `(exp << 31) >> 31`
    //  (arithmetic) reproduces for the low bit's sign-extension.
    int parityMask = (exp << 31) >> 31;
    //  `/ 2`, not `>> 1`.  `parityMask + exp` is always even (the mask is
    //  -(exp & 1)), so the two agree for every input, but the compiler cannot
    //  know that: `/ 2` emits the shipped `cdq` / `sub eax,edx` / `sar eax,1`
    //  round-toward-zero sequence, where `>> 1` folds the shift into the
    //  following `<< 23` as a single `shl 22` / `and` and loses four
    //  instructions.
    int shiftedExp = (parityMask + exp) / 2;
    int resultExp = shiftedExp + 0x7F;
    unsigned int keep = (unsigned int)parityMask & 0x3504F3u;

    CFloat scale((resultExp << 23) | (int)keep);
    //  Two shapes here are the shipped code's, not a preference:
    //    * the product goes through a named local rather than straight into
    //      the return buffer - the shipped code hands operator* a third
    //      temporary and copies its bits out afterwards;
    //    * the multiply is `scale * halfRootBase`, in that order.  It is the
    //      same product either way, but the order decides which of the two
    //      CFloats is the `this` and which is the pushed argument, and the
    //      shipped call has the correction in edx and the root on the stack.
    CFloat product = scale * halfRootBase;
    return product;
}
