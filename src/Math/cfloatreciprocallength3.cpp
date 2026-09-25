//============================================================================
//  0x6F2BAE20 - CFloatReciprocalLength3: 1/sqrt(x*x + y*y + z*z), the
//  reciprocal length of a CFloat 3-vector given as three separate scalar
//  references rather than a vector's address.  The sole caller,
//  NormalizeWorldVec3 (cworldvec3normalize.cpp), passes v->m_x/m_y/m_z.
//
//  `x` arrives in edx and the hidden CFloat return buffer in ecx, exactly
//  like CFloat's own operators (y and z are the two stack arguments, in
//  that order, cleaned by `retn 8`).
//
//  The evaluation order in the dump is z*z, then y*y, then x*x, then
//  (x*x + y*y), then that plus z*z - not left-to-right.  That is exactly
//  what MSVC produces for `x*x + y*y + z*z`: each operator+/operator* here
//  is a real __fastcall, and the compiler evaluates a call's arguments
//  right-to-left, recursively - the outermost `+`'s right operand (z*z) is
//  evaluated before its left operand (x*x + y*y), whose own right operand
//  (y*y) is evaluated before ITS left operand (x*x).  Nothing here needs to
//  be reordered by hand; the natural left-to-right source spelling below
//  reproduces the dump's call order because the compiler already inverts it.
//============================================================================
#include "CFloat.h"

CFloat __fastcall CFloatReciprocalLength3(const CFloat& x, const CFloat& y,
                                          const CFloat& z)
{
    return CFloatInvSqrt(x * x + y * y + z * z);
}
