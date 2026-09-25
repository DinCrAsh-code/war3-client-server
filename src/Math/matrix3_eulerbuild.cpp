//============================================================================
//  0x6F4B2A90 - reached from CUnit vtable slot 110's own root
//  (Method_0x1B8, 0x6F282160, still a thunk - unit_slotthunks.cpp), which
//  builds an orientation from a facing angle and two others; this is the
//  three-angle matrix builder that root's own body calls into.
//
//  Builds three axis-rotation matrices with the same nine-forward-order-
//  store shape matrix3_axisrot.cpp already documents (`_CIcos`/`_CIsin`,
//  not the CFloat table), concatenates them Z*(Y*X) with the same
//  OrientationMatrixMul this file's siblings use, and stores the
//  *transpose* of that product into `this` (nine explicit loads from the
//  product in transposed order, then nine stores - not a `rep movsd` of
//  the product itself, unlike RotateAxis's untransposed copy-back).
//
//  Thiscall, not __fastcall: all three angles are floats, so none of them
//  is register-eligible under __fastcall and the shipped code reads all
//  three off the stack (`retn 0Ch`) with only `this` in ecx - the same
//  reasoning RotateAxis's own header comment gives.
//============================================================================
#include "matrix3.h"

//  No CRT to link against (/Zl); see matrix3_axisrot.cpp for why these are
//  declared instead of #include <math.h>.
double sin(double value);
double cos(double value);
#pragma intrinsic(sin)
#pragma intrinsic(cos)

void OrientationMatrix::BuildFromZYXEulerAngles(float angleZ, float angleY,
                                                float angleX)
{
    SOrientationMatrix rotZ, rotY, rotX, tmp;

    float cosZ = (float)cos(angleZ);
    float sinZ = (float)sin(angleZ);
    rotZ.m[0] = cosZ;  rotZ.m[1] = -sinZ; rotZ.m[2] = 0.0f;
    rotZ.m[3] = sinZ;  rotZ.m[4] = cosZ;  rotZ.m[5] = 0.0f;
    rotZ.m[6] = 0.0f;  rotZ.m[7] = 0.0f;  rotZ.m[8] = 1.0f;

    float cosY = (float)cos(angleY);
    float sinY = (float)sin(angleY);
    rotY.m[0] = cosY;  rotY.m[1] = 0.0f;  rotY.m[2] = -sinY;
    rotY.m[3] = 0.0f;  rotY.m[4] = 1.0f;  rotY.m[5] = 0.0f;
    rotY.m[6] = sinY;  rotY.m[7] = 0.0f;  rotY.m[8] = cosY;

    float cosX = (float)cos(angleX);
    float sinX = (float)sin(angleX);
    rotX.m[0] = 1.0f;  rotX.m[1] = 0.0f;  rotX.m[2] = 0.0f;
    rotX.m[3] = 0.0f;  rotX.m[4] = cosX;  rotX.m[5] = -sinX;
    rotX.m[6] = 0.0f;  rotX.m[7] = sinX;  rotX.m[8] = cosX;

    SOrientationMatrix* mid = OrientationMatrixMul(
        (SOrientationMatrix*)&tmp, (SOrientationMatrix*)&rotY,
        (SOrientationMatrix*)&rotX);
    SOrientationMatrix* result = OrientationMatrixMul(
        (SOrientationMatrix*)&rotY, (SOrientationMatrix*)&rotZ, mid);

    //  Transpose of `result`, built element-by-element and then copied
    //  over *this as one 36-byte struct assignment - the same `rep movsd`
    //  copy-back shape RotateAxis's own last line documents, just fed a
    //  transposed value instead of `product` directly.
    //
    //  Written back into `rotZ` rather than a fifth local: `rotZ` is dead
    //  once the second OrientationMatrixMul call above returns (it was
    //  only ever an input to that call, never read again), and the
    //  shipped code reuses that same 36-byte stack slot for the
    //  transposed result instead of allocating a new one - four buffers
    //  total, not five. Reusing the local directly is what gets the
    //  compiler to do the same; a separately named fifth local measurably
    //  costs a frame-pointer frame here (0x98-byte esp frame vs. an
    //  ebp-based one), which the dump does not have.
    rotZ.m[0] = result->m[0];  rotZ.m[1] = result->m[3];
    rotZ.m[2] = result->m[6];  rotZ.m[3] = result->m[1];
    rotZ.m[4] = result->m[4];  rotZ.m[5] = result->m[7];
    rotZ.m[6] = result->m[2];  rotZ.m[7] = result->m[5];
    rotZ.m[8] = result->m[8];
    *(SOrientationMatrix*)this = rotZ;
}
