//============================================================================
//  0x6F50BFC0 - the sprite's entry into picking: build the model's own
//  bounding sphere in world space, reject the segment against it, and only
//  walk the geometry when it survives.
//
//  The rejection is `closestSquaredDistance <= radius * radius`, and the
//  distance that comes back on the reject path is +infinity rather than
//  FLT_MAX - the two constants are different words in the image and the
//  shipped code uses each in exactly one place.
//============================================================================
#include "modelpick.h"

int __fastcall SpriteModelPick(CModelInstance* model, const float* from,
                               float scale, const float* to, float radius,
                               float* outDistance, int includeChildren)
{
    float sphere[4];
    sphere[0] = 0.0f;
    sphere[1] = 0.0f;
    sphere[2] = 0.0f;
    sphere[3] = 0.0f;

    QueryVariantVec4_6F509260(*(void**)((char*)model + 0x9C), model, sphere);
    TransformBoundingSphere(sphere, (const float*)((char*)model + 0x64), scale);

    ClosestApproach_6F509910(to, sphere, from, &scale, outDistance);

    if (scale <= sphere[3] * sphere[3])
        return ModelPickSegment(model, from, to, outDistance, includeChildren,
                                0);

    *outDistance = g_pickInfinity;
    return 0;
}
