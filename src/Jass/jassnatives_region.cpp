//============================================================================
//  The `region` JASS natives at 0x6F3C14E0..0x6F3C15F0 - six entry points
//  that are really three pairs, each pair differing only in the set/clear
//  flag it passes on.  See jassnatives.h.
//============================================================================
#include "jassnatives.h"

//----------------------------------------------------------------------------
//  0x6F3C1580 / 0x6F3C15F0 - `RegionAddCell` / `RegionClearCell`, both
//  "(Hregion;RR)V": the two reals are passed straight through as pointers,
//  never dereferenced here.
//----------------------------------------------------------------------------
void __cdecl JASS_RegionAddCell(int hRegion, const int* x, const int* y)
{
    SJassRegion* region = ResolveRegionHandle(hRegion);

    if (region)
        region->SetCell(x, y, 1);
}

void __cdecl JASS_RegionClearCell(int hRegion, const int* x, const int* y)
{
    SJassRegion* region = ResolveRegionHandle(hRegion);

    if (region)
        region->SetCell(x, y, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C14E0 / 0x6F3C1530 - `RegionAddRect` / `RegionClearRect`, both
//  "(Hregion;Hrect;)V".  The rect's four bounds are copied onto the stack
//  one dword at a time and the *copy's* address is what SetRect receives,
//  which is why there is a local here and not just `&rect->m_minX`.
//----------------------------------------------------------------------------
void __cdecl JASS_RegionAddRect(int hRegion, int hRect)
{
    SJassRegion* region = ResolveRegionHandle(hRegion);

    if (region)
    {
        SJassRect* rect = ResolveRectHandle(hRect);

        if (rect)
        {
            SJassBounds bounds = *(const SJassBounds*)&rect->m_minX;
            region->SetRect((const int*)&bounds, 1);
        }
    }
}

void __cdecl JASS_RegionClearRect(int hRegion, int hRect)
{
    SJassRegion* region = ResolveRegionHandle(hRegion);

    if (region)
    {
        SJassRect* rect = ResolveRectHandle(hRect);

        if (rect)
        {
            SJassBounds bounds = *(const SJassBounds*)&rect->m_minX;
            region->SetRect((const int*)&bounds, 0);
        }
    }
}
