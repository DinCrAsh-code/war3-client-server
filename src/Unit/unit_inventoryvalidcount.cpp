//============================================================================
//  0x6F26EC00 - CUnit::CountValidInventoryHandles: how many of the
//  inventory ability's own slots still resolve to a live handle, or 0 when
//  the unit has no such ability at all.
//
//  Six instructions, the last of them a tail jump into
//  CAbilityInventory::CountValidHandles - the same "null test is the whole
//  body" shape as GetInventorySize/GetSlotCount right next to it
//  (unit_inventorysize.cpp), and the same reason for its own translation
//  unit: the tail jump is the thing being matched, so this and its target
//  may not be folded together.
//============================================================================
#include "unit.h"
#include "unitinventory.h"

unsigned int CUnit::CountValidInventoryHandles()
{
    CAbilityInventory* inventory = UnitInventory(this);
    if (inventory == 0)
        return 0;

    return inventory->CountValidHandles();
}
