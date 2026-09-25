//============================================================================
//  nullsub_14 @ 0x6F40B1D0 - the empty function SetPlayerUnitsOwner calls,
//  and the whole of that native's effect: one `retn 4`, `this` in ecx and one
//  stack argument neither read nor stored.
//
//  Its own translation unit so that JASS_SetPlayerUnitsOwner keeps the real
//  call the dump has instead of inlining nothing into itself.
//============================================================================
#include "jassnatives.h"

void SJassPlayerUnits::SetOwner(int)
{
}
