//============================================================================
//  0x6F4B2DA0 - the x87 twin of CMatrix3::RotateAxis (0x6F4B00C0,
//  cmatrix3_rotate.cpp).  Same shape exactly: build a rotation from the
//  axis and angle, pre-multiply this matrix by it, copy the 36-byte result
//  back over *this with a `rep movsd`.
//
//  `this` in ecx with the angle, the axis and the flag all on the stack is
//  a thiscall, not a __fastcall - a __fastcall free function would put the
//  axis pointer in edx (a float argument is never register-eligible, so
//  `angle` would be skipped and `axis` promoted), and the shipped code
//  reads the axis out of the stack at +0x8.  Hence the member spelling on
//  matrix3.h's layout-identical OrientationMatrix.
//============================================================================
#include "matrix3.h"

void OrientationMatrix::RotateAxis(float angle, const FVec3& axis,
                                   int alreadyNormalized)
{
    SOrientationMatrix rotation;
    SOrientationMatrix product;
    OrientationMatrixMul(&product,
                         MakeAxisRotationF(&rotation, &axis, angle,
                                           alreadyNormalized),
                         (const SOrientationMatrix*)this);
    *(SOrientationMatrix*)this = product;
}
