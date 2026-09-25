//============================================================================
//  The region membership tests and the fog-modifier / trigger natives.
//  See jassnatives.h.
//============================================================================
#include "jassnatives.h"

//----------------------------------------------------------------------------
//  0x6F3C1690 - `IsPointInRegion` "(Hregion;RR)B": both reals passed through
//  as pointers, unread here.
//----------------------------------------------------------------------------
int __cdecl JASS_IsPointInRegion(int hRegion, const int* x, const int* y)
{
    SJassRegion* region = ResolveRegionHandle(hRegion);

    if (!region)
        return 0;

    return ((SJassRegionTests*)region)->ContainsPoint(x, y);
}

//----------------------------------------------------------------------------
//  0x6F3C1660 - `IsUnitInRegion` "(Hregion;Hunit;)B".  Both handles are
//  resolved *before* either is tested - the shipped code calls the region
//  resolver, parks its answer in esi, calls the unit resolver and only then
//  branches - so both calls happen even when the first one fails.  The unit's
//  own sub-object at +0x164 is what the region is asked about, not the unit.
//----------------------------------------------------------------------------
int __cdecl JASS_IsUnitInRegion(int hRegion, int hUnit)
{
    SJassRegion* region = ResolveRegionHandle(hRegion);
    CUnit* unit = ResolveUnitHandle(hUnit);

    if (!region || !unit)
        return 0;

    return ((SJassRegionTests*)region)->ContainsUnitPosition((char*)unit + 0x164);
}
