//============================================================================
//  0x6F4C6480 / 0x6F4C64E0 - the pair that moves a point between the game's
//  normalised [0,1] screen space and its own UI coordinate space.
//
//  The two constants are the UI space's own extents: 0.8 across and 0.6 down,
//  which is Warcraft III's documented frame coordinate system.  Both are
//  stored as *doubles* holding the value of a `float` literal
//  (0x3FE99999A0000000 is exactly (double)0.8f, 0x3FE3333340000000 exactly
//  (double)0.6f), which is why the multiply is `fmul qword ptr` on a
//  single-precision operand.  dbl_6F93C218 already has a funcmap name from
//  0x6F4C6560, which divides by the same Y span.
//
//  Either output pointer may be null and each is tested separately - the two
//  are not one two-component point.
//============================================================================
#include "uispan.h"

extern double g_uiSpanX;            // dbl_6F8768B0 - 0.8
extern double g_unkDbl6F93C218;     // dbl_6F93C218 - 0.6

//----------------------------------------------------------------------------
//  0x6F4C6480 - normalised -> UI units.
//----------------------------------------------------------------------------
void __fastcall ScaleToUiSpan(float* outX, float* outY, float x, float y)
{
    if (outX != 0)
        *outX = (float)(x * g_uiSpanX);

    if (outY != 0)
        *outY = (float)(y * g_unkDbl6F93C218);
}

//----------------------------------------------------------------------------
//  0x6F4C64E0 - UI units -> normalised.
//----------------------------------------------------------------------------
void __fastcall ScaleFromUiSpan(float* outX, float* outY, float x, float y)
{
    if (outX != 0)
        *outX = (float)(x / g_uiSpanX);

    if (outY != 0)
        *outY = (float)(y / g_unkDbl6F93C218);
}
