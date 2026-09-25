//============================================================================
//  0x6F3C8F00 - JASS_UnitInventorySize, `(Hunit;)I`, and the CUnit member
//  it tail-jumps into.
//
//  The guarded-success shape, and it has to be: all three exits that answer
//  0 share one `xor eax,eax` / `pop esi` / `retn` at the end, and the
//  success path leaves through a *tail jump* into the member rather than a
//  call - `mov ecx,esi` / `pop esi` / `jmp sub_6F26EC60`.  Written as early
//  returns instead, MSVC hoists the zero into the middle and the tail call
//  becomes a call plus a return.  (jassnatives_unitquery.cpp wants exactly
//  the opposite spelling, for exactly the opposite layout.)
//============================================================================
#include "jassnatives.h"
#include "unit.h"
#include "unitinventory.h"

int __cdecl JASS_UnitInventorySize(int hUnit)
{
    CUnit* unit = ResolveUnitHandle(hUnit);
    if (unit && unit->CanUseInventory())
        return unit->GetInventorySize();

    return 0;
}
