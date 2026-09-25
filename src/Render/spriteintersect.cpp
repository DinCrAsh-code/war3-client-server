//============================================================================
//  0x6F4D2D70 - does this sprite's model intersect the segment, and how far
//  along?
//
//  Two of the sprite's own vtable slots are reached: slot 3 evaluates the
//  animation at time 0 when the sprite has not been evaluated yet (flag bit
//  0x200000), and slot 0x11 answers the segment parameter this hands the
//  model.  Both are spelled as slot dispatches rather than as virtuals,
//  because giving CSprite virtuals here would make this build stamp a
//  vtable the shipped code never touches.
//============================================================================
#include "modelpick.h"

int __fastcall SpriteIntersectSegment(CSprite* sprite, const float* from,
                                      const float* to, float radius,
                                      float* outDistance)
{
    if (sprite->m_pModel == 0)
        return 0;

    if ((sprite->m_stateBits & 0x200000) == 0)
    {
        typedef void (__thiscall *EvaluateFn)(CSprite*, float, int, int, int);
        ((EvaluateFn)(*(void***)sprite)[0x0C / 4])(sprite, 0.0f, 0, 0, 1);
    }

    typedef float (__thiscall *ProjectFn)(CSprite*, const float*, float,
                                          float*, int);
    float t = ((ProjectFn)(*(void***)sprite)[0x44 / 4])(sprite, to, radius,
                                                        outDistance, 0);

    return SpriteModelPick(sprite->m_pModel, from, t, to, radius, outDistance,
                           0);
}
