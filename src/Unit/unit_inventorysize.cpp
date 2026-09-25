//============================================================================
//  0x6F26EC60 - CUnit::GetInventorySize: how many item slots the inventory
//  ability at +0x1F8 publishes, or 0 when the unit has no such ability.
//
//  Six instructions, the last of them a tail jump into
//  CAbilityInventory::GetSlotCount - so the null test is the whole body and
//  the answer is the ability's own.  `UnitInventory()` rather than a named
//  member because CUnit's declared layout stops short of +0x1F8, which is
//  the same reason unitinventory.h's own accessors exist.
//
//  Own translation unit, next to GetItemInSlot's: it calls GetSlotCount for
//  real and the tail jump is the thing being matched, so the two may not be
//  folded together.
//============================================================================
#include "unit.h"
#include "unitinventory.h"

int CUnit::GetInventorySize()
{
    CAbilityInventory* inventory = UnitInventory(this);
    if (inventory == 0)
        return 0;

    return (int)inventory->GetSlotCount();
}
