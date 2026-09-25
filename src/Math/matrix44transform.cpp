//============================================================================
//  0x6F4B5450 - out = v * m: a four-component row vector through a row-major
//  4x4.  MultiplyMatrix44's first row, four times over.
//============================================================================
#include "fmatrix.h"

float* __fastcall TransformByMatrix(float* out, const float* v,
                                    const float* matrix)
{
    out[0] = v[0] * matrix[0] + v[1] * matrix[4]
           + v[2] * matrix[8] + v[3] * matrix[12];
    out[1] = v[0] * matrix[1] + v[1] * matrix[5]
           + v[2] * matrix[9] + v[3] * matrix[13];
    out[2] = v[0] * matrix[2] + v[1] * matrix[6]
           + v[2] * matrix[10] + v[3] * matrix[14];
    out[3] = v[0] * matrix[3] + v[1] * matrix[7]
           + v[2] * matrix[11] + v[3] * matrix[15];

    return out;
}
