//============================================================================
//  0x6F47C420 - the remainder of a deterministic float after dividing by a
//  fixed step: `value - step * trunc(value * (1/step))`, with the usual
//  correction that keeps the answer non-negative when `value` is.
//
//  Reached from CWidget's slots 48 and 49 (CUnit::Method_0xC0 /
//  CUnit::Method_0xC4, unit_slotthunks.cpp) and from two more wrappers at
//  0x6F47C490 / 0x6F47C4B0.
//
//  Two module-owned CFloat constants do the work and neither is named
//  anywhere in the image: dword_6FAAE5EC is the reciprocal that turns the
//  multiply into a divide, dword_6FAAE608 the step itself.  They are
//  declared here by address, the way CFloat.h already declares
//  g_CFloatZero, rather than given invented values.
//
//  The sign correction is a *hardware* float compare (`fld` / `fcompp` on
//  the two bit patterns), not one of CFloat's own comparison members - the
//  encoding is IEEE-754 single precision, so the FPU reads it directly, and
//  this is one of the few places the module lets it.  The branch skips the
//  correction when `value >= 0`; the shipped code writes it that way round,
//  which is why the correction sits in the fall-through and not behind an
//  `if (value < 0)`.
//
//  It returns its CFloat *by value*, so `ecx` on entry is the hidden
//  return buffer and `edx` the operand - which is what lets the final
//  subtraction construct straight into the caller's storage.  Reading it as
//  an out-pointer plus a value instead costs a temporary and a copy at the
//  end, and `retn` would still be 0 either way, so nothing but the diff
//  says which it is.
//
//  Own translation unit: four real calls into the software-float library.
//============================================================================
#include "CFloat.h"

//  0x6F6EF290 - floattruncfrac.cpp.
CFloat* __fastcall CFloatTruncateFrac(CFloat* dest, const CFloat* src);

//  The module's own two constants.  Addresses only; see the header comment.
extern const CFloat g_stepReciprocal;   // dword_6FAAE5EC
extern const CFloat g_stepSize;         // dword_6FAAE608

//  Hands back the address of a temporary bound to a const reference - see
//  the call below for why the multiply's result has to travel that way
//  rather than through a named local.
static const CFloat* AddressOf(const CFloat& v) { return &v; }

CFloat __fastcall CFloatModStep(const CFloat& value)
{
    //  The multiply's result reaches CFloatTruncateFrac as the temporary's
    //  own address, which `operator*` has just left in eax - the shipped
    //  `mov edx, eax`.  A named `CFloat scaled = ...` local makes MSVC
    //  `lea` a fresh stack slot instead and costs both the reuse and the
    //  slot.  AddressOf() binds the temporary to a const reference for the
    //  length of the full expression, which is the one spelling that hands
    //  the callee that address without a copy.
    CFloat whole(kCFloatNoInit);
    CFloat offset = g_stepSize *
        *CFloatTruncateFrac(&whole, AddressOf(value * g_stepReciprocal));

    //  A hardware-float compare on the two bit patterns, with `value` as
    //  the left operand so it is the one loaded first (the shipped
    //  `fld [esi]` ahead of `fld g_CFloatZero`).
    if (*(const float*)&value.m_bits < *(const float*)&g_CFloatZero.m_bits)
        offset = offset - g_stepSize;

    //  Returned by value: `out` is the hidden return buffer, which is what
    //  lets this last subtraction construct straight into it (the shipped
    //  `mov ecx, edi` before the call and `mov eax, edi` after it) instead
    //  of building a temporary and copying.
    return value - offset;
}
