//============================================================================
//  0x6F3C5AB0 - the JASS native `SetUnitPositionLoc`,
//  "(Hunit;Hlocation;)V".
//
//  The `location` wrapper around `SetUnitPosition`
//  (jassnatives_unitpositionxy.cpp): resolve the handle, copy the
//  coordinate pair out of +0x24/+0x28 into two locals and hand their
//  addresses on.
//
//  Unlike its `IsLocation...ToPlayer` siblings this one has nothing to
//  return, so the null path is a plain fall-through rather than an early
//  `return 0` - which is why the shipped code branches *past* the body on
//  the null test instead of returning out of it.
//
//  Own translation unit: it calls the XY native for real.
//============================================================================
#include "jassnatives.h"
#include "CFloat.h"

//  jassnatives_unitpositionxy.cpp.
void __cdecl JASS_SetUnitPosition(int hUnit, const int* x, const int* y);

void __cdecl JASS_SetUnitPositionLoc(int hUnit, int hLocation)
{
    SJassLocation* location = ResolveLocationHandle(hLocation);

    if (location)
    {
        CFloat y(location->m_y);
        CFloat x(location->m_x);

        JASS_SetUnitPosition(hUnit, (const int*)&x, (const int*)&y);
    }
}
