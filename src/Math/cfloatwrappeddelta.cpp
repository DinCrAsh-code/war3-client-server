//============================================================================
//  0x6F496630 - a CFloat sign/threshold selector: `diff = a - x`, then one
//  of four raw expressions depending on diff's sign and whether |diff|
//  passes a half-step threshold (`dword_6FAAE600`, already named
//  `g_CFloatPi`/`g_quadrantNorth` elsewhere in this codebase) against a
//  full-step constant (`dword_6FAAE608`, already `g_stepSize` -
//  Math/floatmodstep.cpp).  `retn 4`, `__fastcall`, returned by value (the
//  hidden hidden-buffer-in-ecx shape every CFloat-by-value function in
//  this module has - CFloat.h's own operator- is the model).
//
//  A previous pass could not pin the branch-direction spelling down with
//  confidence; re-derived here straight off the two `fcompp`/`fnstsw`/
//  `test ah,N` pairs using docs/msvc-vc8-idioms.md's own comparison-spelling
//  table (`fld a` / `fld b` / `fcompp` / `test ah,5` / `jp` == `a > b`,
//  applied twice with the loaded-first operand tracked across the
//  intervening `fcomp st(1)` that leaves one operand parked on the FPU
//  stack rather than popping both):
//
//      diff = a - x
//      if (diff < 0)
//          return (|diff| > Pi) ? |diff| : (-stepSize) - diff
//      else
//          return (diff > Pi)  ? -diff  : stepSize - diff
//
//  Both extremes (|diff| past the half-step) collapse to the same
//  `-diff`/`|diff|` raw negation with no further arithmetic; only the
//  inner band calls back into operator- a second time, and only that call
//  differs in which operand is negated (the near/negative branch builds a
//  local `-stepSize` first; the near/positive branch reads `g_stepSize`
//  directly, matching the asm's own `mov edx, offset dword_6FAAE608` with
//  no intervening xor).
//============================================================================
#include "CFloat.h"

//  dword_6FAAE600 - already g_quadrantNorth (Pathfinding/quadrant.h),
//  reached here under that exact existing name/type rather than a fresh
//  local one: funcmap.DATA has exactly one row for this address, and a
//  second name with no row of its own is a real `link_check.py` failure
//  (an "UNEXPECTED unresolved external"), not just a style choice.
extern const CFloat g_quadrantNorth;   // dword_6FAAE600

//  dword_6FAAE608 - already g_stepSize (Math/floatmodstep.cpp).
extern const CFloat g_stepSize;            // dword_6FAAE608

//----------------------------------------------------------------------------
//  0x6F496630.
//----------------------------------------------------------------------------
CFloat __fastcall CFloatWrappedDelta(const CFloat& a, const CFloat& x)
{
    CFloat diff = a - x;

    if (*(const float*)&diff.m_bits < *(const float*)&g_CFloatZero.m_bits)
    {
        CFloat absDiff(diff.m_bits ^ kCFloatSignMask);
        if (*(const float*)&absDiff.m_bits > *(const float*)&g_quadrantNorth.m_bits)
            return absDiff;

        CFloat negStep(g_stepSize.m_bits ^ kCFloatSignMask);
        return negStep - diff;
    }
    else
    {
        if (*(const float*)&diff.m_bits > *(const float*)&g_quadrantNorth.m_bits)
            return CFloat(diff.m_bits ^ kCFloatSignMask);

        return g_stepSize - diff;
    }
}
