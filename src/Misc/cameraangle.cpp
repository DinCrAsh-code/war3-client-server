//============================================================================
//  CAngle - RTTI name only (??_7CAngle@@6B@), one of the nine embedded
//  CBaseManaged-vtabled sub-objects CCamera::CCamera (Misc/ccameraconstruct.cpp,
//  0x6F4E7790) constructs at +0xD0/+0xF8/+0x120/+0x148 with a plain float
//  argument each.  Same field shape as controllerslot.h's own
//  SControllerSlot - vtable, a flag byte at +0x0D, two reserved ints, a
//  float and three more - which the whole "camera controller slot" family
//  already shares; CAngle just uses the last two of the three trailing
//  floats as a cached sin/cos pair instead of a vec3, and derives from
//  CBaseManaged instead of carrying its vtable/link inline.
//
//  0x6F4E76C0 - CAngle::CAngle(float value).  Zero every derived field,
//  hand SetValue the address of the constructor's own stack argument, done.
//  The base CBaseManaged() default-constructs its TSLink first (the two
//  zero stores at +0x04/+0x08 ahead of anything else) - its own vtable
//  stamp is a dead store the compiler drops because CAngle's own stamp
//  overwrites it before anything reads it back, which is also why only one
//  `mov dword ptr [ecx], offset ...` shows up in the dump despite two
//  vtables being logically written.
//
//  The whole function carries the `__except_handler4`-shaped SEH frame
//  docs/msvc-vc8-idioms.md's "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce" already covers: a base class with a real
//  (non-trivial) destructor forces the compiler to protect the constructor
//  body so an exception unwinding through it still destroys the
//  already-constructed CBaseManaged base (the scope table's own trampoline,
//  at an unrelated address in the dump, does exactly `((CBaseManaged*)obj)
//  ->~CBaseManaged()`, i.e. `m_link.Unlink()`).  This build's bundled
//  compiler cannot reach that exact frame shape under any flag combination
//  that session tried - it always gets the older EBP-based
//  `__except_handler3` form instead - so this stays `DIFFERS` on the frame
//  alone regardless of how faithfully the body matches.
//
//  0x6F4E6C10 - CAngle::SetValue(const float* pValue): wrap `*pValue`
//  through NormalizeAngle (0x6F4E7EA0), update the cached normalized value
//  and OR in flag bit 3 only when it actually changed (a plain `!=`
//  compare - the mask-0x44/`jnp` NaN-safe shape docs/msvc-vc8-idioms.md's
//  "A float-vs-zero compare's status-word mask" section documents, here
//  against another float rather than a literal 0.0f), then refresh the
//  cached sin/cos off the (possibly still old) normalized value
//  unconditionally.  No SEH frame - the ordinary, everything-after-this-
//  point-observable case.
//============================================================================
#include "cameraangle.h"

//  No CRT to link against (/Zl) - same declaration-plus-pragma shape
//  Math/matrix3_axisrot.cpp already uses for the x87 sin/cos intrinsics.
double sin(double value);
double cos(double value);
#pragma intrinsic(sin)
#pragma intrinsic(cos)

//  0x6F4E7EA0 - wrap an angle into (-pi,pi].  Round-trip through
//  angle * 1/(2*pi) -> (int) -> * 2*pi, branching on the sign of the input
//  to pick which of the two truncation directions keeps the result in
//  range: C's `(int)` cast truncates toward zero, which floors a positive
//  quotient but ceils a negative one, so the negative arm subtracts one
//  more multiple to compensate.  The two named constants are this build's
//  own literal-pool floats; their addresses never appear in either side of
//  the diff (canonlib maps every unresolved data reference, on both sides
//  alike, to `SYM`), so only their values matter and only for behaviour,
//  not score.
static float __stdcall NormalizeAngle(float angle)
{
    const float kInvTwoPi = 0.15915494f;   // 1 / (2*pi)
    const float kTwoPi    = 6.28318531f;   // 2*pi

    int n = (int)(kInvTwoPi * angle);
    float rounded = (float)n * kTwoPi;
    if (angle >= 0.0f)
        return angle - rounded;
    return angle - (rounded - kTwoPi);
}

//  0x6F4E6C10.
void CAngle::SetValue(const float* pValue)
{
    float normalized = NormalizeAngle(*pValue);
    if (m_normalizedValue != normalized)
    {
        m_flags |= 8;
        m_normalizedValue = normalized;
    }
    m_sin = (float)sin(m_normalizedValue);
    m_cos = (float)cos(m_normalizedValue);
}

//  0x6F4E76C0.
CAngle::CAngle(float value)
{
    m_rawValue = 0.0f;
    m_reserved0C = 0;
    m_flags = 0;
    m_field10 = 0;
    m_field14 = 0;
    SetValue(&value);
}
