//============================================================================
//  The hardware-`float` matrix routines - the twins of cmatrix3.h's CFloat
//  ones, in the 0x6F4Bxxxx neighbourhood the render pipeline uses.
//
//  Nothing here declares a matrix or vector *type*.  All three take and
//  return `float*`, which is what the shipped ABI actually is: the receiver
//  is a hidden return buffer in ecx, the first operand in edx and the second
//  on the stack, with the buffer handed back in eax - the shape
//  docs/msvc-vc8-idioms.md's calling-convention table gives for a
//  `__fastcall` free function returning a class by value.  A `float*`
//  spelling reproduces it exactly and does not invent a class name the
//  binary does not carry one for: none of the three addresses reaches an
//  RTTI descriptor or a vtable stamp.
//
//  Storage is row-major throughout: the 4x4 is 4 rows of 4 and the 4x3 is 4
//  rows of 3, the last of which is the translation.  That is what the index
//  arithmetic below says and it is the same convention CMatrix3 already has.
//============================================================================
#ifndef FMATRIX_H
#define FMATRIX_H

//  0x6F4B5050 - out = a * b, 4x4 by 4x4.  Math/matrix44mul.cpp.
float* __fastcall MultiplyMatrix44(float* out, const float* a,
                                   const float* b);

//  0x6F4B5450 - out = v * m, a four-component row vector through a 4x4.
//  Math/matrix44transform.cpp.
float* __fastcall TransformByMatrix(float* out, const float* v,
                                    const float* matrix);

//  0x6F4B6D50 - out = v * m + m.row3, a three-component point through a 4x3.
//  Math/matrix43transform.cpp.
float* __fastcall TransformPointByMatrix43(float* out, const float* v,
                                           const float* matrix);

#endif
