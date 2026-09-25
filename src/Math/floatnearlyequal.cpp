//============================================================================
//  0x6F467E50 - "are these two within a fixed tolerance of each other".
//
//  |a - b| against g_unk6FAAE61C, the same leftover epsilon
//  AdvanceSchedulerAndDrain tests against.  It really does go through
//  CFloat::operator- and then mask the sign bit off the *result's* stored
//  pattern rather than comparing the two directly - which is why this one is
//  a call and CFloat::IsLess (floatcompare.cpp) is not.
//
//  `__fastcall`: the shipped body reads its first operand out of ecx and its
//  second out of edx and pushes nothing, and then hands operator- exactly
//  those two.  It also narrows its answer through `movzx eax, al`, so the
//  0/1 is a byte on the way out.
//============================================================================
#include "CFloat.h"

//  flt_6FAAE61C - the same tolerance src/scheduler_drain.cpp tests against.
extern const float g_unk6FAAE61C;

int __fastcall CFloatNearlyEqual(const CFloat& a, const CFloat& b)
{
    //  Not through a named `CFloat difference` local: the shipped code masks
    //  the sign bit off the pattern it reads back through the pointer
    //  operator- returned (`mov eax,[eax]`), and naming the result makes
    //  this build read its own slot instead.
    int magnitude = (a - b).m_bits & 0x7FFFFFFF;

    //  Written magnitude-first.  MSVC emits the two `fld`s in source order,
    //  and the shipped body loads the magnitude before the tolerance - which
    //  is the `test ah,41h` / `jne` form rather than the `test ah,5` / `jp`
    //  one the other way round produces.
    int near;
    if (*(const float*)&magnitude < g_unk6FAAE61C)
        near = 1;
    else
        near = 0;

    return (unsigned char)near;
}
