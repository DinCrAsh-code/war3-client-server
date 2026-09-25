//============================================================================
//  0x6F4B16F0 - concatenate two hardware-float 3x3 matrices: the x87 twin of
//  cmatrix3_mul.cpp's CFloat operator* (0x6F4AF4D0).  Same row-major
//  convention, same result[i][j] = sum_k a[i][k]*b[k][j].
//
//  Own translation unit for the same reason as the CFloat one: its only
//  caller here, Matrix3::RotateAxis (0x6F4B2DA0, matrix3_rotate.cpp), calls
//  it for real rather than inlining it.
//
//  Written as nine separate stores through a result pointer rather than as
//  a nine-argument constructor - the reverse of what cmatrix3_mul.cpp needs.
//  With x87 there is no hidden return buffer to fill in argument order:
//  the shipped code computes element 0 first and stores it immediately
//  (`fstp dword ptr [eax]` at 0x6F4B170B, before element 1 is even loaded),
//  so the natural forward-order spelling is the matching one here.
//
//  `a` arrives in edx and the result buffer in ecx, which is what a
//  __fastcall free function taking (result, a) gives; `b` is the one stack
//  argument (retn 4).
//
//  The nine rows are spelled in the plain a-then-b order.  The shipped
//  listing loads `b`'s operand first in twenty of the twenty-seven products
//  and computes the middle term before the first one in seven of the nine
//  rows, but that is x87 operand selection, not source: transcribing the
//  shipped order literally - every product and every term written in the
//  order the disassembly computes it - was measured and produced *the same
//  86 instructions and the same 48 matches*.  Both spellings compile to the
//  same thing, so the readable one is what is in the file.  See
//  docs/msvc-vc8-idioms.md, "x87 operand order is not source-addressable".
//============================================================================
#include "matrix3.h"

SOrientationMatrix* __fastcall OrientationMatrixMul(SOrientationMatrix* result,
                                                   const SOrientationMatrix* a,
                                                   const SOrientationMatrix* b)
{
    result->m[0] = a->m[0] * b->m[0] + a->m[1] * b->m[3] + a->m[2] * b->m[6];
    result->m[1] = a->m[0] * b->m[1] + a->m[1] * b->m[4] + a->m[2] * b->m[7];
    result->m[2] = a->m[0] * b->m[2] + a->m[1] * b->m[5] + a->m[2] * b->m[8];
    result->m[3] = a->m[3] * b->m[0] + a->m[4] * b->m[3] + a->m[5] * b->m[6];
    result->m[4] = a->m[3] * b->m[1] + a->m[4] * b->m[4] + a->m[5] * b->m[7];
    result->m[5] = a->m[3] * b->m[2] + a->m[4] * b->m[5] + a->m[5] * b->m[8];
    result->m[6] = a->m[6] * b->m[0] + a->m[7] * b->m[3] + a->m[8] * b->m[6];
    result->m[7] = a->m[6] * b->m[1] + a->m[7] * b->m[4] + a->m[8] * b->m[7];
    result->m[8] = a->m[6] * b->m[2] + a->m[7] * b->m[5] + a->m[8] * b->m[8];
    return result;
}
