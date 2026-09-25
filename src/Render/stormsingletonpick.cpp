//============================================================================
//  0x6F762310 - SStormSingletonA::QueryObstruction: the nearest registered
//  object along the segment `from` -> `to`.
//
//  The direction is normalised and then *checked*: the shipped code squares
//  the normalised vector, subtracts 1.0 and rejects the whole query when the
//  result is not within 0.001 of zero.  That is a guard against a
//  zero-length segment, whose normalisation leaves NaNs behind - and it is
//  written as `!(|x| < eps)` rather than `>=` because that is the sense the
//  `fcomp` / `test ah, 5` / `jnp` pair has.
//
//  What comes back through `outDistance` is the *unnormalised* parameter,
//  and the hit point is `from + (to - from) * (t / length)`, which is why
//  the length is kept from before the normalisation.
//============================================================================
#include "stormsingletona.h"
#include "modelpick.h"

double sqrt(double value);
//  The two differ, and both spellings are load-bearing.  `fabs` only
//  substitutes as an intrinsic on a *C-linkage* declaration: with C++
//  linkage MSVC warns C4162 and emits a real `?fabs@@YANN@Z` that nothing on
//  the link line defines.  `sqrt` is the other way round - C++ linkage
//  substitutes to `_CIsqrt`, which the CRT region provides, and `extern "C"`
//  substitutes to `__CIsqrt`, which it does not.  Both were live link
//  failures before they were spelled this way.
extern "C" double fabs(double value);
#pragma intrinsic(sqrt, fabs)

//  0x6F75E870 - Render/objectpick.cpp.  Declared exactly as that file
//  defines it: a member, so the owner rides in ecx and all five arguments
//  are on the stack where the shipped function cleans them.
struct PICKOWNER
{
    int Pick(void* object, const float* origin, const float* direction,
             float* outDistance, int includeShapes);
};

int SStormSingletonA::QueryObstruction(const float* from, const float* to,
                                       float* outHitPoint, float* outDistance,
                                       int flags)
{
    float hit[3];
    hit[0] = 0.0f;
    hit[1] = 0.0f;
    hit[2] = 0.0f;

    int found = 0;

    float delta[3];
    delta[0] = to[0] - from[0];
    delta[1] = to[1] - from[1];
    delta[2] = to[2] - from[2];

    float scratch = delta[0] * delta[0] + delta[1] * delta[1] +
                    delta[2] * delta[2];
    scratch = (float)sqrt(scratch);
    float length = scratch;

    FVec3 direction;
    direction.m_x = delta[0];
    direction.m_y = delta[1];
    direction.m_z = delta[2];
    direction.Normalize();

    scratch = direction.m_x * direction.m_x + direction.m_y * direction.m_y +
              direction.m_z * direction.m_z;
    scratch = (float)(scratch - 1.0);
    scratch = (float)fabs(scratch);

    if (!(scratch < g_cameraTolerance))
        return 0;

    if (m_pickCount == 0)
        return 0;

    float t = 0.0f;
    float best = g_pickInfinity2;

    for (unsigned int i = 0; i < m_pickCount; i++)
    {
        if (((PICKOWNER*)this)->Pick(m_pickObjects[i], from,
                                     &direction.m_x, &t, flags) != 0)
        {
            found = 1;
            if (t < best)
                best = t;
        }
    }

    if (found == 0)
        return 0;

    float u = best / length;

    hit[0] = delta[0] * u + from[0];
    hit[1] = delta[1] * u + from[1];
    hit[2] = delta[2] * u + from[2];

    if (outHitPoint != 0)
    {
        outHitPoint[0] = hit[0];
        outHitPoint[1] = hit[1];
        outHitPoint[2] = hit[2];
    }

    if (outDistance != 0)
        *outDistance = best;

    return found;
}
