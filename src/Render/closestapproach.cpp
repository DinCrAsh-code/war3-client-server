//============================================================================
//  0x6F509910 - the closest approach between the ray `origin`->`b` and the
//  point `a`, as the two floats the caller asked for.
//
//  Both differences are taken against `origin` first: `d1 = a - origin` and
//  `d2 = b - origin`.  The parameter along `d2` is `dot(d2, d1) /
//  dot(d2, d2)`, clamped to the closed interval - the shipped code takes
//  the three arms in the order `t <= 0`, `t >= 1`, and the divide - and what
//  comes back is the residual's squared length and the parameter.
//
//  See modelpick.h for the register convention: three pointer inputs in eax,
//  edx and ecx is one more than `__fastcall` has registers, so the third is
//  a stack parameter here and this body cleans twelve bytes where the
//  shipped one cleans eight.
//============================================================================
#include "modelpick.h"

void __fastcall ClosestApproach_6F509910(const float* a, const float* b,
                                         const float* origin, float* out0,
                                         float* out1)
{
    float d1[3];
    float d2[3];

    d1[0] = b[0] - origin[0];
    d1[1] = b[1] - origin[1];
    d1[2] = b[2] - origin[2];

    d2[0] = a[0] - origin[0];
    d2[1] = a[1] - origin[1];
    d2[2] = a[2] - origin[2];

    float t = d2[0] * d1[0] + d2[1] * d1[1] + d2[2] * d1[2];

    if (0.0f >= t)
    {
        t = 0.0f;
    }
    else
    {
        float len2 = d2[0] * d2[0] + d2[1] * d2[1] + d2[2] * d2[2];
        if (len2 <= t)
        {
            t = 1.0f;
            d1[0] = d1[0] - d2[0];
            d1[1] = d1[1] - d2[1];
            d1[2] = d1[2] - d2[2];
        }
        else
        {
            t = t / len2;
            d1[0] = d1[0] - d2[0] * t;
            d1[1] = d1[1] - d2[1] * t;
            d1[2] = d1[2] - d2[2] * t;
        }
    }

    *out0 = d1[0] * d1[0] + d1[1] * d1[1] + d1[2] * d1[2];
    *out1 = t;
}
