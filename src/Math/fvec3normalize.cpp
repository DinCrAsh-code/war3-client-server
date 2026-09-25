//============================================================================
//  0x6F011550 - normalize a plain (hardware) 3-float vector in place: divide
//  each component by sqrt(x*x + y*y + z*z).  This is real x87 arithmetic,
//  not CFloat - the shipped code reads/writes the fields with `fld`/`fst`
//  and calls the compiler's own `_CIsqrt` helper, which is what `sqrt()`
//  compiles to here rather than a real CRT call (this build has no CRT to
//  link against, /Zl, so the intrinsic substitution is what makes the call
//  resolve at all).
//============================================================================

//  The compiler's own square-root intrinsic: `sqrt()` on a double argument,
//  with no <math.h> to declare it, compiles to `call _CIsqrt` - the value
//  arrives and returns on the x87 stack, not through any register or stack
//  argument, which is why the declaration takes no parameters that show up
//  in the generated call.  `#pragma intrinsic` forces the substitution
//  regardless of which optimisation flags a given build uses `cl.exe`
//  with - left implicit, matching_decomp_injection's own build (different
//  flags from tools/build.sh) linked against a genuine, unresolved
//  external `sqrt` instead of `_CIsqrt`.
double sqrt(double value);
#pragma intrinsic(sqrt)

#include "fvec3.h"

void FVec3::Normalize()
{
    float x = m_x;
    float y = m_y;
    float z = m_z;
    //  One scratch local, overwritten three times (sum of squares, then the
    //  root, then the reciprocal) - the shipped code reuses a single 4-byte
    //  stack slot the same way.
    float scratch = x * x + y * y + z * z;
    scratch = (float)sqrt(scratch);
    scratch = 1.0f / scratch;

    m_x = m_x * scratch;
    m_y = m_y * scratch;
    m_z = m_z * scratch;
}
