//============================================================================
//  0x6F022270 - FloatValueIsFarFrom.
//
//  "Has this FloatMini's value dropped to `target` yet?"  Answered as: no,
//  it is still above it *and* the gap is at least g_quadrantEpsilon wide.
//  Below the target, or within an epsilon of it, the answer is 0 - which is
//  what makes it the "we have arrived" test both of CItem's value setters
//  (item_valueset.cpp) gate their depletion path on.
//
//  A __fastcall free function and not a FloatMini member: the shipped code
//  takes the target pointer in edx and cleans nothing (`retn`), where a
//  thiscall member would have taken it on the stack and cleaned four bytes
//  - the exact `ret <n>` mismatch tools/abi_audit.py exists to catch.
//
//  Both comparisons are spelled as *negations* (`!(a < b)` rather than
//  `a >= b`) because the shipped code masks the x87 status word the way
//  MSVC only does for a negated compare: `a >= b` emits `test ah,41h` with
//  `jp` on the false edge, `!(a < b)` emits the same mask with `je`, and
//  it is `je` that is in the binary.  The two differ only for NaN, which is
//  precisely why the compiler keeps them apart - see
//  docs/msvc-vc8-idioms.md.
//============================================================================
#include "floatmini.h"
#include "quadrant.h"       // g_quadrantEpsilon - flt_6FAAE47C

//  By value, not by const reference: the by-value parameter is what makes
//  MSVC materialise a *second* copy of each operand for the FPU compare,
//  which is the shipped code's own double store of both slots (once for
//  the two `fld`s, once again for operator-'s two reference arguments).
static inline float AsHardwareFloat(CFloat value) { return *(const float*)&value; }

bool __fastcall FloatValueIsFarFrom(const FloatMini* self, const CFloat* target)
{
    //  No named CFloat locals: the shipped code holds both encoded values
    //  in registers for the whole function and spills each of them to the
    //  *same* stack slot twice - once for its `fld`, once again for
    //  operator-'s reference argument.  That only happens when the two
    //  spills are separate, non-overlapping temporaries the allocator can
    //  coalesce; a named local whose address is taken later stays live
    //  across the compare and gets a slot of its own, which costs four
    //  instructions and a bigger frame.
    if (AsHardwareFloat(self->m_value) > AsHardwareFloat(*target))
    {
        //  The explicit copies are what create those second temporaries -
        //  passing `self->m_value` straight to operator- would hand it the
        //  member's own address and spill nothing.  The sign bit is then
        //  cleared on the *encoded* result, a plain integer AND for
        //  IEEE-754 storage (the shipped `and ecx, 7FFFFFFFh`), read
        //  straight out of the returned buffer rather than through a named
        //  local of its own.
        CFloat magnitude(
            (int)((CFloat(self->m_value) - CFloat(*target)).m_bits & 0x7FFFFFFF));

        if (!(AsHardwareFloat(magnitude) < g_quadrantEpsilon))
            return true;
    }

    return false;
}

//----------------------------------------------------------------------------
//  0x6F0222E0/0x6F022340/0x6F0221F0 - three more members of this same
//  family, all reached from the CAbility legacy-tag remap switch
//  (ability_typemigrate.cpp, slot 19, 0x6F040AE0).  Same magnitude-vs-
//  g_quadrantEpsilon shape as FloatValueIsFarFrom above; unlike it, all
//  three keep the *positive* sense of the compare (`magnitude < epsilon`
//  rather than a negated `!(a<b)`), which is why their own `fnstsw`/
//  `test ah,41h` pairs branch the opposite way.
//
//  FloatMiniNearlyEquals/FloatMiniDiffers are exact opposites of each
//  other (`near`/`!near`) reached from the same call shape at every site
//  in that switch; the redundant `test al,al` / `setnz|setz cl` / `mov
//  al,cl` after the plain 1-or-0 assign is the shipped code's own -
//  narrowing an `int` result through an explicit `!= 0`/`== 0` rather
//  than returning the flag straight out the way FloatValueIsFarFrom does.
//----------------------------------------------------------------------------
int __fastcall FloatMiniNearlyEquals(const FloatMini* self, const CFloat* target)
{
    CFloat magnitude(
        (int)((CFloat(self->m_value) - CFloat(*target)).m_bits & 0x7FFFFFFF));

    int near = (AsHardwareFloat(magnitude) < g_quadrantEpsilon);
    return near != 0;
}

int __fastcall FloatMiniDiffers(const FloatMini* self, const CFloat* target)
{
    CFloat magnitude(
        (int)((CFloat(self->m_value) - CFloat(*target)).m_bits & 0x7FFFFFFF));

    int near = (AsHardwareFloat(magnitude) < g_quadrantEpsilon);
    return near == 0;
}

//  A third comparison ORed onto the first: within-epsilon *or* strictly
//  greater, i.e. "is target at least self->m_value" - reached only from
//  the two slot-19 cases that gate a save-format version float rather
//  than a plain legacy tag constant.
int __fastcall FloatMiniAtMost(const FloatMini* self, const CFloat* target)
{
    CFloat magnitude(
        (int)((CFloat(self->m_value) - CFloat(*target)).m_bits & 0x7FFFFFFF));

    if (AsHardwareFloat(magnitude) < g_quadrantEpsilon)
        return 1;

    return AsHardwareFloat(*target) > AsHardwareFloat(self->m_value);
}
