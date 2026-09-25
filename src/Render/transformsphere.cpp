//============================================================================
//  0x6F5085F0 - a bounding sphere through a 4x3: the centre transformed by
//  the matrix, the radius multiplied by the scale.
//
//  The matrix arrives in eax in the shipped build - the whole-program
//  convention - so it is the edx parameter here and the caller pays one
//  move.  The transform's result is copied back out of the buffer it filled
//  because TransformPointByMatrix43 returns a class by value; the radius is
//  multiplied first and stored last, which is the shipped order.
//============================================================================
#include "modelpick.h"
#include "fmatrix.h"

void __fastcall TransformBoundingSphere(float* sphere, const float* matrix,
                                        float scale)
{
    float centre[3];
    const float* moved = TransformPointByMatrix43(centre, sphere, matrix);

    float radius = sphere[3] * scale;

    sphere[0] = moved[0];
    sphere[1] = moved[1];
    sphere[2] = moved[2];
    sphere[3] = radius;
}
