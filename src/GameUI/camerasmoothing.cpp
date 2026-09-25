//============================================================================
//  0x6F302760 - CCameraWar3::SetSmoothingFactor: take the JASS native
//  `CameraSetSmoothingFactor`'s argument, clamp it into
//  [0, g_cameraMaxSmoothingFactor] and store it at +0x5BC.
//
//  Both clamps are hardware-float compares against the *incoming argument's
//  own stack slot*, which is written back on each of the three paths and
//  reloaded once at the end - the `fstp [esp+arg_0]` / `fld [esp+arg_0]`
//  pair before the single store.  That is assignment to the parameter, not
//  a temporary: a local would get a slot of its own and the store would come
//  off the x87 stack directly.
//
//  The upper bound is an unnamed constant in the shipped .rdata
//  (flt_6F92ED10) and is declared by address, the way CFloat.h already
//  declares g_CFloatZero, rather than given an invented value.
//
//  Own translation unit: no calls at all, but 0x6F302710 sits in the same
//  0x6F302xxx block and is a real call from the two field writers.
//============================================================================
#include "gameui.h"

//  flt_6F92ED10 - the largest smoothing factor the camera accepts.
extern const float g_cameraMaxSmoothingFactor;

void CCameraWar3::SetSmoothingFactor(float factor)
{
    if (factor > 0.0f)
    {
        if (factor >= g_cameraMaxSmoothingFactor)
            factor = g_cameraMaxSmoothingFactor;
    }
    else
    {
        factor = 0.0f;
    }

    m_smoothingFactor = factor;
}

//  flt_6FA75F24 - the half-turn-shaped limit WrapToRange folds its argument
//  back inside; funcmap.py DATA as g_cameraWrapRange.
extern const float g_cameraWrapRange;

//----------------------------------------------------------------------------
//  0x6F302710 - fold `x` back into (-g_cameraWrapRange, g_cameraWrapRange]
//  by truncating division, the same C-style `fmodf` shape
//  `x - limit * (float)(int)(x / limit)` uses everywhere else in this repo.
//  Two guarded compares (`x` against +/-limit) rather than one `fabsf`
//  check: the shipped stream never takes an absolute value at all, just
//  negates one loaded copy of the constant and compares twice - see
//  docs/msvc-vc8-idioms.md's float-comparison mask table for the exact
//  `test ah, 41h` / `jp` shape both compares share (spelled here as `>=`
//  both times, which is what a two-limb `&&` range check compiles to).
//----------------------------------------------------------------------------
float __fastcall WrapToRange(float x)
{
    if (x >= -g_cameraWrapRange && x < g_cameraWrapRange)
        return x;

    return x - g_cameraWrapRange * (float)(int)(x / g_cameraWrapRange);
}
