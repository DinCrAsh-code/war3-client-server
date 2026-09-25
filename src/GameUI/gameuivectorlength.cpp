//============================================================================
//  0x6F301BE0 - FVec3::Length.
//
//  sqrt(x*x + y*y + z*z) in real x87 arithmetic.  The declaration and its
//  `#pragma intrinsic` are spelled exactly as Math/fvec3normalize.cpp has
//  them, and - like that file - the pragma does *not* substitute: MSVC warns
//  C4162 ("no function with C linkage found") and emits a real,
//  stack-argument `call ?sqrt@@YANN@Z`, which Math/sqrtfallback.cpp exists
//  to keep linkable, where the shipped code has `call _CIsqrt` with the
//  value on the x87 stack.  That costs five of the six missing
//  instructions.
//
//  Adding `extern "C"` to the declaration *does* make the pragma
//  substitute - measured this session: the function goes to 18 of 19, with
//  only the call operand left (an extern "C" EXTRN carries no readable-name
//  comment for extract_gen.py to resolve).  It is not landed because the
//  resulting `__CIsqrt` is an unresolved external nothing on the .mix's link
//  line defines - tools/link_check.py fails on it - so it needs a redirect
//  for `_CIsqrt` first.  See docs/msvc-vc8-idioms.md.
//
//  The `fstp`/`fld` pair either side of the call is the sum being narrowed
//  to a real `float` local and then widened again for the call: the shipped
//  code stores it, which a nameless subexpression would not do.
//
//  Own translation unit: its two callers (the screen projections at
//  0x6F333CB0 and 0x6F334180) call it for real.
//============================================================================
double sqrt(double value);
#pragma intrinsic(sqrt)

#include "fvec3.h"

float FVec3::Length() const
{
    float lengthSquared = m_x * m_x + m_y * m_y + m_z * m_z;

    return (float)sqrt(lengthSquared);
}
