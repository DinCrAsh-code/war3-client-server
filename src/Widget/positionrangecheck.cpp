//============================================================================
//  0x6F476C40 - SHandleWithType::IsWithinRange: is the world point (x, y)
//  within `range` of the path trace this handle names?
//
//  The same {handle, typeTag} pair SPositionRadiusSink::SetRadius
//  (positionradiussink.cpp) resolves, at the same +0x08/+0x0C, through the
//  same ordinary LookupHandle - and it resolves to a CPathTrace, which is
//  what its two calls on the result settle (GetTravelDistance 0x6F497460
//  and PointAtDistance 0x6F38B0B0 are both CPathTrace members already
//  reconstructed in CPathTrace.cpp).
//
//  All of the arithmetic is CFloat, and all of it in *grid* space: the
//  range is scaled down by game.h's WorldToGrid and the point is offset by
//  the map origin and scaled the same way.  What is compared is the square
//  of the distance against the square of (range + the trace's own radius),
//  so nothing ever takes a square root.
//
//  `fromTravelPoint` picks which end of the trace to measure from: set, the
//  point the trace has actually travelled to by now
//  (PointAtDistance(GetTravelDistance())); clear, the trace's own origin at
//  +0x78.  JASS's IsUnitInRange family always passes 1.
//
//  The tail is an epsilon-tolerant `<=`, spelled exactly the way
//  FloatValueIsFarFrom (floatmini_compare.cpp) spells its own: both
//  comparisons are *negations* (`!(a < b)`, not `a >= b`), because that is
//  what makes MSVC emit `test ah,41h` with `je` on the false edge rather
//  than `jp`, and `je` is what is in the binary.  The two differ only for
//  NaN, which is exactly why the compiler keeps them apart.
//
//  Own translation unit: five real calls out of it, two of them into
//  CPathTrace.
//============================================================================
#include "timesyncbounds.h"
#include "game.h"
#include "quadrant.h"       // g_quadrantEpsilon - flt_6FAAE47C

//  0x6F6EF3B0 - Math/softfloat_distsq.cpp; also declared in pathmove.h,
//  which this file does not need for anything else.
CFloat __fastcall CFloatDistanceSquared(const CGridVec2& a,
                                        const CGridVec2& b);

//  By value, for the reason floatmini_compare.cpp gives: the by-value
//  parameter is what makes MSVC materialise the operand the FPU compare
//  needs without keeping the named local alive across it.
static inline float AsHardwareFloat(CFloat value)
{
    return *(const float*)&value;
}

int SHandleWithType::IsWithinRange(const CFloat* range, const CFloat* x,
                                   const CFloat* y, int fromTravelPoint)
{
    CPathTrace* trace = (CPathTrace*)LookupHandle(m_handle, m_typeTag);

    //  The radius is copied into a temporary rather than handed over by
    //  address: the shipped code stores +0x90 into a stack slot of its own
    //  and passes that slot, which only an explicit copy produces.
    CFloat reach = WorldToGrid(*range);
    reach = reach + CFloat(trace->m_radius);
    CFloat reachSq = reach * reach;

    //  Constructor arguments are evaluated right to left, which is why the
    //  y axis is converted first and lands in the dead `y` parameter's own
    //  stack slot while x gets a temporary of its own.
    CGridVec2 pos(kCFloatNoInit);
    pos.m_y = WorldToGrid(*y - g_pGameData->m_originY);
    pos.m_x = WorldToGrid(*x - g_pGameData->m_originX);

    CFloat distSq(kCFloatNoInit);
    if (fromTravelPoint)
        distSq = CFloatDistanceSquared(
            pos, trace->PointAtDistance(trace->GetTravelDistance()));
    else
        distSq = CFloatDistanceSquared(pos, trace->m_origin);

    if (!(AsHardwareFloat(distSq) < AsHardwareFloat(reachSq)))
    {
        //  The sign bit cleared on the *encoded* difference, a plain
        //  integer AND on IEEE-754 storage (the shipped `and eax,
        //  7FFFFFFFh`), read straight out of the returned buffer.
        CFloat gap((int)((distSq - reachSq).m_bits & 0x7FFFFFFF));

        if (!(AsHardwareFloat(gap) < g_quadrantEpsilon))
            return 0;
    }

    return 1;
}
