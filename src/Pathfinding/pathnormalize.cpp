//============================================================================
//  0x6F49B050 - normalize a direction vector, but only if it is long enough
//  to divide by safely.
//
//  The shipped build takes `pt` in esi and `magnitude` in ebx, using `this`
//  (ecx) for nothing at all - two parameters in registers no declarable
//  calling convention reaches (docs/msvc-vc8-idioms.md, "Arguments in eax
//  and esi"), so this reconstruction takes them as ordinary parameters and
//  is IDENTICAL rather than EXACT once linked into a caller.
//
//  Always computes the vector's length; only divides pt's own components
//  by it when the length clears g_slopeThreshold1 (the same encoded-1.0
//  constant pathgridpoint.cpp's ClassifySlopeAxis compares against),
//  leaving a too-short vector untouched rather than risk a near-zero
//  divide.
//============================================================================
#include "pathmove.h"

static inline float AsHardwareFloat(CFloat value) { return *(const float*)&value; }

CFloat NormalizeVec2(CGridVec2* pt)
{
    CFloat lenSq = pt->m_y * pt->m_y + pt->m_x * pt->m_x;
    CFloat len = CFloatSqrt(lenSq);

    if (AsHardwareFloat(len) > AsHardwareFloat(g_slopeThreshold1))
    {
        CFloat recip = CFloatReciprocal(len);
        pt->m_x = pt->m_x * recip;
        pt->m_y = pt->m_y * recip;
    }

    return len;
}
