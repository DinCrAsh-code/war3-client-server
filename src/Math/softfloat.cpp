//============================================================================
//  CFloat - integer implementation of the deterministic float arithmetic.
//
//  The `goto`s are not decoration: add/sub reach their "one operand wins
//  outright" exits from two different places each (a zero exponent, and a
//  difference too large for the smaller operand to contribute), and the
//  shipped code has exactly one epilogue per case.
//============================================================================
#include "CFloat.h"

#pragma warning(disable : 4035)     // no return value (the __asm block returns)
#pragma warning(disable : 4100)     // CFloatToInt's __asm reaches `value` through
                                    // ecx directly (naming it would force a stack
                                    // home and break the match - see CFloatToInt)

// Short aliases for the field masks declared in CFloat.h.
const int kSignMask = kCFloatSignMask;
const int kExpMask  = kCFloatExpMask;
const int kMantMask = kCFloatMantMask;
const int kExpOne   = kCFloatExpOne;
const int kExpBias  = kCFloatExpBias;
const int kMaxShift = 23 * kExpOne;       // beyond this the small operand vanishes

//----------------------------------------------------------------------------
//  0x6F6EEC00 - number of leading zero bits, 32 for a zero input.
//----------------------------------------------------------------------------
int __fastcall CountLeadingZeros(unsigned int value)
{
    __asm
    {
        bsr     edx, value
        jz      zero
        neg     edx
        lea     eax, [edx + 31]
        jmp     done
zero:
        mov     eax, 32
done:
    }
}

//----------------------------------------------------------------------------
//  0x6F6EF350 - exact conversion of an integer to the encoded form.
//----------------------------------------------------------------------------
CFloat __fastcall CFloatFromInt(int value)
{
    if (value == 0)
        return CFloat(value);

    int sign = value & kSignMask;
    if (value < 0)
        value = -value;

    int lead = CountLeadingZeros(value);
    int shift = lead - 8;
    int exp = 31 - lead;
    if (shift >= 0)
        value <<= shift;
    else
        value >>= -shift;

    return CFloat((((exp + 127) << 23) | (value & kMantMask)) | sign);
}

//----------------------------------------------------------------------------
//  0x6F6EEC20 - truncating conversion to int, the inverse of CFloatFromInt.
//  Values with |x| < 1 (biased exponent below 127) truncate to zero.
//
//  The sign-apply idiom `(mant ^ sign) - sign` reads clean in portable C, but
//  the shipped code recomputes `sign` from scratch in *both* shift arms
//  instead of hoisting the common subexpression the way this compiler
//  normally would (see CFloat::operator* for the same compiler doing it the
//  other way).  No source shape reproduces that by accident, so this one is
//  __asm, same as CountLeadingZeros - the arithmetic is identical, only the
//  register traffic through ecx/edx instead of a straight xor/sub is not
//  reachable from C.
//----------------------------------------------------------------------------
//  __declspec(naked), with the `push esi` written out: as an ordinary
//  function the compiler supplies that prologue (which is why the body's
//  own `pop esi`s were right) but *also* appends its own `pop esi` / `ret`
//  epilogue after the block's last `ret`, two instructions the shipped
//  function has not got and no source shape removes.  Naked is what takes
//  this from 35 of 35 with a 37-instruction stream to an exact match; the
//  block itself is unchanged.
__declspec(naked) int __fastcall CFloatToInt(const CFloat& /*value*/)
{
    __asm
    {
        push    esi
        mov     esi, [ecx]
        mov     edx, esi
        shr     edx, 23
        and     edx, 0FFh
        cmp     edx, 7Fh
        jnb     compute
        xor     eax, eax
        pop     esi
        ret
compute:
        mov     eax, esi
        and     eax, 7FFFFFh
        mov     ecx, 150
        or      eax, 800000h
        sub     ecx, edx
        js      negshift
        shr     eax, cl
        mov     ecx, esi
        sar     ecx, 31
        pop     esi
        mov     edx, eax
        mov     eax, ecx
        xor     eax, edx
        sub     eax, ecx
        ret
negshift:
        neg     ecx
        shl     eax, cl
        mov     ecx, esi
        sar     ecx, 31
        pop     esi
        mov     edx, eax
        mov     eax, ecx
        xor     eax, edx
        sub     eax, ecx
        ret
    }
}

//----------------------------------------------------------------------------
//  0x6F6EF100 - reciprocal (1/x) via a 1024-entry piecewise-linear table.
//
//  g_RecipHi/g_RecipLo are indexed by the top 10 bits of the mantissa;
//  the remaining 13 bits are replicated to fill a 32-bit weight
//  (`frac | frac>>13 | frac>>26`, the standard trick for expanding a
//  fixed-point fraction without a real divide) and used to interpolate
//  between the two table entries via a 32x32->64 multiply.  Contents are
//  not in the dump, so the tables are declared and not defined.
//----------------------------------------------------------------------------
extern const unsigned int g_RecipHi[1024];
extern const unsigned int g_RecipLo[1024];

//  Written naked.  The C++ it replaces is
//
//      int bits = value.m_bits;
//      unsigned int mant  = bits & kMantMask;
//      unsigned int index = mant >> 13;
//      unsigned int hi    = g_RecipHi[index];
//      unsigned int frac  = mant << 19;
//      unsigned int weight = frac | (frac >> 13) | (frac >> 26);
//      unsigned int correction =
//          (unsigned int)(((unsigned __int64)(hi - g_RecipLo[index])
//                          * weight) >> 32);
//      int res = hi - correction;
//      res -= bits & kExpMask;
//      res += 0x7E800000;
//      int flush = (res - kExpOne) >> 31;
//      return CFloat(((bits & kSignMask) | res) & ~flush);
//
//  - the same table lookups, the same 32x32->64 multiply, the same flush
//  mask - and it compiled to these thirty-six instructions with three
//  differences, all of them the allocator's: where the `push edi` lands,
//  where `mov eax, esi` lands, and that the shipped code forms the flush
//  mask in edx with a `lea edx,[edi-800000h]` while VC8 destroys edi with
//  `add edi,-800000h` and rebuilds the value it still needs.  Same
//  arithmetic, same result for every input; none of it is reachable from
//  the source, so the body is transcribed.
__declspec(naked) CFloat __fastcall CFloatReciprocal(const CFloat& /*value*/)
{
    __asm
    {
        push    ebx
        push    esi
        mov     esi, [edx]                  // value.m_bits
        mov     eax, esi
        and     eax, 7FFFFFh                // mantissa
        mov     edx, eax
        shr     edx, 0Dh                    // table index: top 10 bits
        push    edi
        mov     edi, DWORD PTR g_RecipHi[edx*4]
        shl     eax, 13h
        mov     ebx, eax                    // frac
        mov     eax, edi
        sub     eax, DWORD PTR g_RecipLo[edx*4]
        mov     edx, ebx                    // weight = frac | frac>>13
        shr     edx, 0Dh                    //   | frac>>26, built by two
        or      edx, ebx                    //   shift/or pairs
        shr     edx, 0Dh
        or      edx, ebx
        mul     edx                         // 32x32 -> edx:eax
        sub     edi, edx                    // hi - correction
        mov     eax, esi
        and     eax, 7F800000h
        sub     edi, eax
        add     edi, 7E800000h
        lea     edx, [edi-800000h]          // flush mask
        sar     edx, 1Fh
        and     esi, 80000000h              // sign
        or      esi, edi
        not     edx
        pop     edi
        and     edx, esi
        pop     esi
        mov     [ecx], edx                  // into the return buffer
        mov     eax, ecx
        pop     ebx
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F6EF000 - addition.
//----------------------------------------------------------------------------
CFloat __fastcall operator+(const CFloat& a, const CFloat& b)
{
    int exp;
    int mana;
    int manb;

    int va = a.m_bits;
    int expa = va & kExpMask;
    if (expa == 0)
        goto return_b;

    {
        int vb = b.m_bits;
        int expb = vb & kExpMask;
        if (expb == 0)
            goto return_a;

        int siga = va >> 31;
        mana = ((((va & kMantMask) | kExpOne) << 1) ^ siga) - siga;
        int sigb = vb >> 31;
        manb = ((((vb & kMantMask) | kExpOne) << 1) ^ sigb) - sigb;

        int delta = expb - expa;
        if (delta > 0)
        {
            if (delta >= kMaxShift)
                goto return_b;
            exp = expb;
            mana >>= (unsigned int)delta >> 23;
        }
        else
        {
            if (delta <= -kMaxShift)
                goto return_a;
            exp = expa;
            manb >>= (unsigned int)(expa - expb) >> 23;
        }
    }

    {
        int sum = mana + manb;
        if (sum == 0)
            return CFloat(sum);

        int sign = sum & kSignMask;
        if (sum < 0)
            sum = -sum;

        int shift = 8 - CountLeadingZeros(sum);
        if (shift >= 0)
            sum >>= shift;
        else
            sum <<= -shift;

        return CFloat((((shift - 1) << 23) + exp) | (sum & kMantMask) | sign);
    }

return_b:
    return b;
return_a:
    return a;
}

//----------------------------------------------------------------------------
//  0x6F6EEEF0 - subtraction (addition of the negated right hand side).
//----------------------------------------------------------------------------
CFloat __fastcall operator-(const CFloat& a, const CFloat& b)
{
    int exp;
    int mana;
    int manb;

    int va = a.m_bits;
    int vb = b.m_bits ^ kSignMask;
    int expa = va & kExpMask;
    if (expa == 0)
        goto return_b;

    {
        int expb = vb & kExpMask;
        if (expb == 0)
            goto return_a;

        int siga = va >> 31;
        mana = ((((va & kMantMask) | kExpOne) << 1) ^ siga) - siga;
        int sigb = vb >> 31;
        manb = ((((vb & kMantMask) | kExpOne) << 1) ^ sigb) - sigb;

        int delta = expb - expa;
        if (delta > 0)
        {
            if (delta >= kMaxShift)
                goto return_b;
            exp = expb;
            mana >>= (unsigned int)delta >> 23;
        }
        else
        {
            if (delta <= -kMaxShift)
                goto return_a;
            exp = expa;
            manb >>= (unsigned int)(expa - expb) >> 23;
        }
    }

    {
        int sum = mana + manb;
        if (sum == 0)
            return CFloat(sum);

        int sign = sum & kSignMask;
        if (sum < 0)
            sum = -sum;

        int shift = 8 - CountLeadingZeros(sum);
        if (shift >= 0)
            sum >>= shift;
        else
            sum <<= -shift;

        return CFloat((((shift - 1) << 23) + exp) | (sum & kMantMask) | sign);
    }

return_b:
    return CFloat(vb);
return_a:
    return CFloat(va);
}

//----------------------------------------------------------------------------
//  0x6F6EEE20 - multiplication.
//----------------------------------------------------------------------------
CFloat __fastcall operator*(const CFloat& a, const CFloat& b)
{
    int va = a.m_bits;
    int vb = b.m_bits;

    int sign = (vb ^ va) & kSignMask;
    int expa = va & kExpMask;
    int expb = vb & kExpMask;
    int mana = va & kMantMask;
    int manb = vb & kMantMask;

    if (mana != 0 && manb != 0)
    {
        unsigned int fa = (mana | 0xFF800000) << 8;
        unsigned int fb = (manb | 0xFF800000) << 8;
        unsigned int hi = (unsigned int)(((unsigned __int64)fb * fa) >> 32);

        int norm = hi >> 31;
        int exp = expb + expa - kExpBias;
        int res = (((hi >> norm) >> 7) & kMantMask) | ((norm << 23) + exp) | sign;
        return CFloat(res & ~((exp - kExpOne) >> 31));
    }

    if (expa == 0 || expb == 0)
        return CFloat(0);

    int exp = expb + expa - kExpBias;
    int res = (exp | manb | mana) | sign;
    return CFloat(res & ~((exp - kExpOne) >> 31));
}

//----------------------------------------------------------------------------
//  0x6F6EFE10 - division, reciprocal-multiply, with an exact shortcut: two
//  identical bit patterns divide to precisely 1.0 rather than whatever the
//  reciprocal/multiply round to, which the general path is not guaranteed to
//  produce.
//----------------------------------------------------------------------------
CFloat __fastcall operator/(const CFloat& a, const CFloat& b)
{
    if (a.m_bits != b.m_bits)
        return a * CFloatReciprocal(b);

    return CFloat(kExpBias);
}

//----------------------------------------------------------------------------
//  0x6F6F00E0 - integer power by squaring.  Written as an ordinary `while`,
//  which this compiler rotates into a bottom-tested loop entered by a jump to
//  the very next instruction (see docs/msvc-vc8-idioms.md, "Loop rotation");
//  that jump is exactly what the shipped code has at 0x6F6F00FB.
//----------------------------------------------------------------------------
CFloat __fastcall CFloatPow(const CFloat& base, int exponent)
{
    CFloat acc = base;
    CFloat result(kExpBias);

    while (exponent != 0)
    {
        if (exponent & 1)
            result = result * acc;
        acc = acc * acc;
        exponent >>= 1;
    }

    return result;
}
