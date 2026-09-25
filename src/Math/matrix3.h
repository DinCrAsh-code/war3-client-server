//============================================================================
//  The hardware-float 3x3 matrix routines - the x87 twins of cmatrix3.h's
//  CFloat ones.  See that header for why the binary carries both.
//
//  These are free functions over itemplacementdispatch.h's existing
//  SOrientationMatrix rather than members of it, so that adding this target
//  does not touch the header three already-matching translation units
//  include.  RotateAxis is the one that genuinely needs `this` in ecx with
//  nothing in edx; it is spelled as a member of a layout-identical
//  OrientationMatrix below and reached by cast, which is what produces a
//  thiscall without disturbing SOrientationMatrix itself.
//============================================================================
#ifndef MATRIX3_H
#define MATRIX3_H

#include "itemplacementdispatch.h"

//  0x6F4B16F0 - matrix3_mul.cpp.
SOrientationMatrix* __fastcall OrientationMatrixMul(SOrientationMatrix* result,
                                                   const SOrientationMatrix* a,
                                                   const SOrientationMatrix* b);

//  0x6F4B2C50 - matrix3_axisrot.cpp.  Result buffer in ecx, axis in edx, the
//  angle and the already-normalized flag on the stack - the mirror image of
//  MakeAxisRotation's own argument order, which takes the angle in edx.
SOrientationMatrix* __fastcall MakeAxisRotationF(SOrientationMatrix* result,
                                                 const FVec3* axis,
                                                 float angle,
                                                 int alreadyNormalized);

//  0x6F4B2DA0 - matrix3_rotate.cpp.
class OrientationMatrix
{
public:
    //  Identity, like CMatrix3's - CWidget::CreateSpriteFromDescriptor
    //  builds one of these on the stack with `fld1`/`fldz` and no call.
    OrientationMatrix()
    {
        m[0] = 1.0f; m[1] = 0.0f; m[2] = 0.0f;
        m[3] = 0.0f; m[4] = 1.0f; m[5] = 0.0f;
        m[6] = 0.0f; m[7] = 0.0f; m[8] = 1.0f;
    }

    void RotateAxis(float angle, const FVec3& axis, int alreadyNormalized);

    //  0x6F4B2A90 - matrix3_eulerbuild.cpp.  `this` in ecx as the
    //  destination, all three angles on the stack (`retn 0Ch`), same
    //  thiscall reasoning as RotateAxis above.
    void BuildFromZYXEulerAngles(float angleZ, float angleY, float angleX);

    float m[9];
};

#endif
