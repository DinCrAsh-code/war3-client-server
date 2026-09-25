//============================================================================
//  0x6F3C9940 / 0x6F3C99D0 / 0x6F3C9A60 - `IsLocationVisibleToPlayer`,
//  `IsLocationFoggedToPlayer` and `IsLocationMaskedToPlayer`, all
//  "(Hlocation;Hplayer;)B".
//
//  Each is the `location` wrapper around the coordinate-pair native of the
//  same name (jassnatives_xyvisibility.cpp): resolve the handle, copy the
//  pair out of the record at +0x24/+0x28 into two locals, and pass their
//  addresses on - the same by-pointer convention every JASS `real` argument
//  in this module uses.
//
//  The pair is read y first and x second, which is the shipped load order
//  (`mov ecx,[eax+28h]` before `mov edx,[eax+24h]`) and the same order
//  JASS_MoveLocation reads them in (jassnatives_location.cpp) - naming both
//  values is what produces it.
//
//  Own translation unit: they call the three XY natives for real.
//============================================================================
#include "jassnatives.h"
#include "CFloat.h"

//  jassnatives_xyvisibility.cpp - `const CFloat*`, not the `const int*` the
//  rest of this module uses for a JASS real; see that file for why.
int __cdecl JASS_IsVisibleToPlayer(const CFloat* x, const CFloat* y, int hPlayer);
int __cdecl JASS_IsFoggedToPlayer(const CFloat* x, const CFloat* y, int hPlayer);
int __cdecl JASS_IsMaskedToPlayer(const CFloat* x, const CFloat* y, int hPlayer);

int __cdecl JASS_IsLocationVisibleToPlayer(int hLocation, int hPlayer)
{
    SJassLocation* location = ResolveLocationHandle(hLocation);

    if (!location)
        return 0;

    CFloat y(location->m_y);
    CFloat x(location->m_x);

    return JASS_IsVisibleToPlayer(&x, &y, hPlayer);
}

int __cdecl JASS_IsLocationFoggedToPlayer(int hLocation, int hPlayer)
{
    SJassLocation* location = ResolveLocationHandle(hLocation);

    if (!location)
        return 0;

    CFloat y(location->m_y);
    CFloat x(location->m_x);

    return JASS_IsFoggedToPlayer(&x, &y, hPlayer);
}

int __cdecl JASS_IsLocationMaskedToPlayer(int hLocation, int hPlayer)
{
    SJassLocation* location = ResolveLocationHandle(hLocation);

    if (!location)
        return 0;

    CFloat y(location->m_y);
    CFloat x(location->m_x);

    return JASS_IsMaskedToPlayer(&x, &y, hPlayer);
}
