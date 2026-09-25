//============================================================================
//  0x6F3C8140 - the JASS native `IsUnitInRangeLoc`,
//  "(Hunit;Hlocation;R)B".
//
//  The `location` wrapper around `IsUnitInRangeXY`
//  (jassnatives_unitrangexy.cpp): resolve the handle, copy the coordinate
//  pair out of +0x24/+0x28, and pass the two addresses plus the caller's
//  own range pointer straight through.
//
//  Own translation unit: it calls the XY native for real.
//============================================================================
#include "jassnatives.h"
#include "CFloat.h"

//  jassnatives_unitrangexy.cpp.
int __cdecl JASS_IsUnitInRangeXY(int hUnit, const CFloat* x, const CFloat* y,
                                 const CFloat* range);

int __cdecl JASS_IsUnitInRangeLoc(int hUnit, int hLocation,
                                  const CFloat* range)
{
    SJassLocation* location = ResolveLocationHandle(hLocation);

    if (!location)
        return 0;

    CFloat y(location->m_y);
    CFloat x(location->m_x);

    return JASS_IsUnitInRangeXY(hUnit, &x, &y, range);
}
