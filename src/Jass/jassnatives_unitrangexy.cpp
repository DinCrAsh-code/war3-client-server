//============================================================================
//  0x6F3C80E0 - the JASS native `IsUnitInRangeXY`, "(Hunit;RRR)B".
//
//  Resolve the unit, build the world point out of the two reals plus the
//  simulation's encoded zero, and ask the unit's own handle object
//  (slot 46, +0xB8) whether the point is within `range`
//  (positionrangecheck.cpp).  The trailing 1 is that function's
//  `fromTravelPoint` flag.
//
//  The point is a *named local* here, not the by-value temporary
//  jassnatives_xyvisibility.cpp builds: this callee takes the two
//  components by address, so the vector has to have one.
//
//  Own translation unit, split from `IsUnitInRangeLoc`
//  (jassnatives_unitrangeloc.cpp), which calls it for real.
//============================================================================
#include "jassnatives.h"
#include "unit.h"
#include "timesyncbounds.h"
#include "game.h"

int __cdecl JASS_IsUnitInRangeXY(int hUnit, const CFloat* x, const CFloat* y,
                                 const CFloat* range)
{
    CUnit* unit = (CUnit*)ResolveUnitHandle(hUnit);

    if (!unit)
        return 0;

    CWorldVec3 pos(*x, *y, g_CFloatZero);

    //  The handle object is *named* before the call: the shipped code
    //  issues the virtual before pushing any of IsWithinRange's four
    //  arguments, and that is the order a named receiver forces - the same
    //  distinction jassnatives_locationmake.cpp records for its own tail.
    SHandleWithType* handle = unit->GetHandleObject();

    return handle->IsWithinRange(range, &pos.m_x, &pos.m_y, 1);
}
