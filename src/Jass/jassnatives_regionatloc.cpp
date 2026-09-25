//============================================================================
//  0x6F3C15B0 - `RegionAddCellAtLoc`, in its own translation unit.
//
//  It calls JASS_RegionAddCell (jassnatives_region.cpp) for real - the dump
//  has `call sub_6F3C1580` and a `__cdecl` three-argument stack cleanup, not
//  an inlined copy of that native's body - so the two cannot share a TU or
//  MSVC folds the callee in and the call disappears.
//============================================================================
#include "jassnatives.h"

void __cdecl JASS_RegionAddCell(int hRegion, const int* x, const int* y);

//----------------------------------------------------------------------------
//  0x6F3C15B0 - `RegionAddCellAtLoc` "(Hregion;Hlocation;)V".  It resolves
//  the location itself and then calls RegionAddCell above by name, pointers
//  and all - the shipped code really does `call sub_6F3C1580` rather than
//  reaching SetCell directly, so the two natives are one call apart and the
//  local coordinate pair exists to give those pointers something to point at.
//----------------------------------------------------------------------------
void __cdecl JASS_RegionAddCellAtLoc(int hRegion, int hLocation)
{
    SJassLocation* location = ResolveLocationHandle(hLocation);

    if (location)
    {
        //  y before x, matching the shipped load order (+0x28 then +0x24).
        int y = location->m_y;
        int x = location->m_x;

        JASS_RegionAddCell(hRegion, &x, &y);
    }
}

