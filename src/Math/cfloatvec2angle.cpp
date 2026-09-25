//============================================================================
//  A small "2D vector -> angle" pair, reached from `NIpse::CPoPos::Slot20`
//  (0x6F4A74A0, Agent/popos_slots1920_thunk.cpp) via its velocity pair
//  (+0x80/+0x84) - part of the shared `CFloat` division/threshold helper
//  web `CPoPos`'s own header comment (Agent/popos.h) flags, alongside
//  Slot19's own still-unresolved half (see that file's own header comment
//  for why Slot19 itself stays a THUNK).
//
//  `SCFloatVec2` is the same "flat, offset-addressed" convention
//  Agent/popos.h's own header comment already documents for `CRlProperty` -
//  not a real class either side reconstructs from, just a name for the
//  (x, y) pair every call site here reaches through a raw field offset
//  (`this+0x80`/`this+0x84` from `CPoPos::Slot20`).
//============================================================================
#include "CFloat.h"

//  0x6F6EF750 - Pathfinding/pathmove.h.
CFloat __fastcall CFloatSqrt(const CFloat& value);

//  0x6F6EFB00 - CFloat.h.

//  dword_6FAAE608 - already g_stepSize (Math/floatmodstep.cpp), a full
//  angular step (2*pi, from this file's own use as "wrap an acos result
//  computed for a negative-y vector back into the upper half").
extern const CFloat g_stepSize;

struct SCFloatVec2
{
    CFloat x;   // +0x00
    CFloat y;   // +0x04

    //  0x6F4B2DE0 - resolve this vector's own angle from its x component
    //  and an already-computed `length`, via `acos(x / length)` - correcting
    //  the quadrant ambiguity `acos` alone cannot resolve using the sign of
    //  `y` (a negative `y` wraps the result into the upper half via
    //  `g_stepSize - angle`, matching a full [0, 2*pi) convention) - and
    //  clamping to `g_CFloatZero` whenever `length` or the raw `acos` result
    //  falls within `0x3727C5AC` (a small fixed epsilon, inline in the
    //  shipped code rather than a named module constant) of zero, the two
    //  degenerate cases (near-zero length; an already axis-aligned vector)
    //  that would otherwise feed `operator/`/`acos` a value they cannot
    //  usefully resolve. `retn 8`: `this` in `ecx`, both real arguments on
    //  the stack (no register beyond `ecx` is read anywhere in the body).
    void ComputeAngle(CFloat* out, const CFloat* length) const;

    //  0x6F1D2AB0 - `length = sqrt(x*x + y*y)`, then `ComputeAngle` above.
    //  Returns `out` unchanged, the same fluent "hand back the output
    //  pointer" convention several sibling helpers in this family already
    //  use.
    CFloat* AngleFromLength(CFloat* out) const;
};

void SCFloatVec2::ComputeAngle(CFloat* out, const CFloat* length) const
{
    if ((unsigned int)(length->m_bits & 0x7FFFFFFF) <= 0x3727C5ACu)
    {
        *out = g_CFloatZero;
        return;
    }

    CFloat angle = CFloatAcos(x / *length);

    if ((unsigned int)(angle.m_bits & 0x7FFFFFFF) <= 0x3727C5ACu)
    {
        *out = g_CFloatZero;
        return;
    }

    if (*(const float*)&y.m_bits < *(const float*)&g_CFloatZero.m_bits)
        *out = g_stepSize - angle;
    else
        *out = angle;
}

CFloat* SCFloatVec2::AngleFromLength(CFloat* out) const
{
    CFloat lenSq = x * x + y * y;
    CFloat length = CFloatSqrt(lenSq);
    ComputeAngle(out, &length);
    return out;
}
