//============================================================================
//  0x6F01FE40 - "are these two within a fixed tolerance of each other", and
//  the *second* copy of that function in this image.  The unit factory
//  (0x6F29F990, jasscreateunit.h) is what reaches this one.
//
//  It is 0x6F467E50 (floatnearlyequal.cpp) instruction for instruction with
//  one operand changed: the tolerance is flt_6FAAE614 rather than
//  flt_6FAAE61C.  Two constants, two bodies, no /OPT:ICF - the same reason
//  agentdefaults.cpp gives for every other duplicated body here - so it
//  needs its own name and its own translation unit rather than being folded
//  into that one.
//
//  Everything the sibling's own comments establish applies unchanged and is
//  not repeated: the unnamed `(a - b).m_bits` (naming the difference makes
//  this build read its own slot instead of the pointer operator- returned),
//  the magnitude-first `fld` order that produces `test ah,41h` / `jne`
//  rather than `test ah,5` / `jp`, and the `movzx eax, al` narrowing that
//  the `(unsigned char)` cast is there for.  Read that file first; this one
//  is deliberately a transcription of it.
//============================================================================
#include "CFloat.h"

//  flt_6FAAE614 - this copy's tolerance.  Four bytes below the one
//  0x6F467E50 uses, and a real `float`: it is reached with a bare `fld`,
//  never through a CFloat operation.
extern const float g_CFloatEqualityTolerance;

int __fastcall CFloatNearlyEqualTight(const CFloat& a, const CFloat& b)
{
    int magnitude = (a - b).m_bits & 0x7FFFFFFF;

    int near;
    if (*(const float*)&magnitude < g_CFloatEqualityTolerance)
        near = 1;
    else
        near = 0;

    return (unsigned char)near;
}
