//============================================================================
//  0x6F4D3070 - push an animation time scale at the sprite's model instance.
//
//  Its own translation unit because it calls RescaleModelTimeline
//  (0x6F501D40, modelinstance.cpp) for real.
//
//  The cast is to `unsigned int`, and that is load-bearing: the shipped
//  code saves the x87 control word, ORs in 0xC00 (round toward zero),
//  converts with `fistp` and restores, which is the block MSVC emits for an
//  *unsigned* destination only - a signed `(int)` compiles to a bare
//  `jmp __ftol2_sse` instead.  See docs/msvc-vc8-idioms.md, "A float-to-
//  `unsigned` truncation is inline; a float-to-`int` one is a call".  It is
//  also what keeps the model instance live in ecx across the conversion,
//  since there is no call to clobber it.  The
//  scale factor is a *double* constant (dbl_6F875AA0, loaded with `fmul`
//  against a qword), so the multiply happens in double precision before the
//  truncation.
//============================================================================
#include "sprite.h"

extern const double g_timeScaleUnitsPerSecond;   // dbl_6F875AA0

void __fastcall SetSpriteTimeScale(CSprite* self, float seconds)
{
    CModelInstance* model = self->m_pModel;
    if (model != 0)
        RescaleModelTimeline(model,
                             (unsigned int)(seconds * g_timeScaleUnitsPerSecond));
}
