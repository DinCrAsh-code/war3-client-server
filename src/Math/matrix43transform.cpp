//============================================================================
//  0x6F4B6D50 - out = v * m + m.row3: a three-component *point* through a
//  row-major 4x3, whose fourth row is the translation.  The `fadd` of the
//  translation rather than a fourth `fmul` is what says the vector has an
//  implied w of 1 and the matrix only three columns.
//============================================================================
#include "fmatrix.h"

float* __fastcall TransformPointByMatrix43(float* out, const float* v,
                                           const float* matrix)
{
    out[0] = v[0] * matrix[0] + v[1] * matrix[3]
           + v[2] * matrix[6] + matrix[9];
    out[1] = v[0] * matrix[1] + v[1] * matrix[4]
           + v[2] * matrix[7] + matrix[10];
    out[2] = v[0] * matrix[2] + v[1] * matrix[5]
           + v[2] * matrix[8] + matrix[11];

    return out;
}
