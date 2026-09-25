//============================================================================
//  0x6F4B2C50 - the x87 twin of MakeAxisRotation (0x6F4AFCD0,
//  cmatrix3_axisrot.cpp): Rodrigues' rotation formula again, same
//  row-vector convention, same nine elements.
//
//  Three real differences from the CFloat one, all of them in the shipped
//  code rather than chosen here:
//
//  * The argument order is mirrored - the result buffer is still in ecx,
//    but the *axis* arrives in edx and the angle on the stack, where the
//    CFloat entry takes the angle in edx and the axis on the stack.  One
//    template instantiated twice could not do that; these are two
//    hand-maintained copies.
//  * sin and cos come from two separate _CIsin/_CIcos intrinsic calls
//    instead of the simulation's single table-driven CFloatSinCos, so the
//    angle is evaluated twice.
//  * The nine elements are nine forward-order assignments through the
//    result pointer, not a nine-argument constructor: with x87 there is no
//    hidden return buffer being filled in argument order, and the shipped
//    code stores element 0 before it has even loaded element 1's operands.
//============================================================================
#include "matrix3.h"

//  No CRT to link against (/Zl), so these compile to the compiler's own
//  x87 helpers rather than real calls - same declaration-plus-pragma shape
//  fvec3normalize.cpp uses for sqrt().
double sin(double value);
double cos(double value);
#pragma intrinsic(sin)
#pragma intrinsic(cos)

SOrientationMatrix* __fastcall MakeAxisRotationF(SOrientationMatrix* result,
                                                 const FVec3* axis,
                                                 float angle,
                                                 int alreadyNormalized)
{
    FVec3 n = *axis;
    if (!alreadyNormalized)
        n.Normalize();

    float s = (float)sin(angle);
    float c = (float)cos(angle);

    float xx = n.m_x * n.m_x;
    float yy = n.m_y * n.m_y;
    float zz = n.m_z * n.m_z;
    float xy = n.m_x * n.m_y;
    float yz = n.m_y * n.m_z;
    float zx = n.m_z * n.m_x;
    float xs = n.m_x * s;
    float ys = n.m_y * s;
    float zs = n.m_z * s;
    float t = 1.0f - c;

    result->m[0] = t * xx + c;
    result->m[1] = t * xy + zs;
    result->m[2] = t * zx - ys;
    result->m[3] = t * xy - zs;
    result->m[4] = t * yy + c;
    result->m[5] = t * yz + xs;
    result->m[6] = t * zx + ys;
    result->m[7] = t * yz - xs;
    result->m[8] = t * zz + c;
    return result;
}
