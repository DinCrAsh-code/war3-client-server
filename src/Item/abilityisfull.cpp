//============================================================================
//  0x6F0F0E80 - CAbilityInventory::IsFull: this ability's inventory is full
//  when the number of occupied slots equals the level-capped slot count.
//
//  Own translation unit: the module is 0x6F0F0Exxx, distinct from both
//  callees' own modules (CountValidHandles at 0x6F0E3xxx,
//  GetSlotCount at 0x6F0E4050 - both abilityinventoryslot.cpp) and from
//  StoreItem's (0x6F0FD040), which is its only caller in this call tree -
//  not called here to keep it out of line, the same way
//  abilityslotcount.cpp's own note explains for GetSlotCount.
//
//  CountValidHandles is the same address (0x6F0E3DB0) the CUnit
//  inventory-closure batch's own DetachAndNotify reaches too
//  (abilityinventoryslot.cpp) - one reconstruction, two independent
//  callers.
//============================================================================
#include "unitinventory.h"

int CAbilityInventory::IsFull()
{
    return CountValidHandles() == GetSlotCount();
}
