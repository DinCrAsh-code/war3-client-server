//============================================================================
//  0x6F3C1620 / 0x6F3C16B0 - the two location-taking region natives, each in
//  a call to the coordinate-taking native next to it.  Kept out of
//  jassnatives_region.cpp / jassnatives_region2.cpp for the same reason
//  RegionAddCellAtLoc is: the shipped code calls those natives for real, and
//  sharing a TU lets MSVC inline them and lose the call.
//============================================================================
#include "jassnatives.h"

void __cdecl JASS_RegionClearCell(int hRegion, const int* x, const int* y);
int  __cdecl JASS_IsPointInRegion(int hRegion, const int* x, const int* y);

//  0x6F3C1620 - `RegionClearCellAtLoc` "(Hregion;Hlocation;)V".
void __cdecl JASS_RegionClearCellAtLoc(int hRegion, int hLocation)
{
    SJassLocation* location = ResolveLocationHandle(hLocation);

    if (location)
    {
        //  y before x, matching the shipped load order (+0x28 then +0x24).
        int y = location->m_y;
        int x = location->m_x;

        JASS_RegionClearCell(hRegion, &x, &y);
    }
}

//  0x6F3C16B0 - `IsLocationInRegion` "(Hregion;Hlocation;)B".
int __cdecl JASS_IsLocationInRegion(int hRegion, int hLocation)
{
    SJassLocation* location = ResolveLocationHandle(hLocation);

    if (!location)
        return 0;

    int y = location->m_y;
    int x = location->m_x;

    return JASS_IsPointInRegion(hRegion, &x, &y);
}
