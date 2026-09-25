//============================================================================
//  0x6F0FD040 - CAbilityInventory::StoreItem: place `item` into the first
//  slot that is either empty (the `(typeTag & handle) == -1` bit check) or
//  whose handle no longer resolves, up to GetSlotCount() slots.
//
//  Own translation unit: a real out-of-line call to
//  CAbilityInventory::StoreItemIntoSlot (0x6F0F8C10, abilitystoreslot.cpp)
//  every iteration, so the two cannot be in a position to be inlined into
//  each other.
//
//  The bit check is evaluated before the handle-resolve call and
//  short-circuits it exactly the way `||` does here: the shipped code jumps
//  straight past the `call sub_6F4786B0` when the slot is already flagged
//  empty by its bit pattern.
//============================================================================
#include "unitinventory.h"

int CAbilityInventory::StoreItem(void* item, int notify)
{
    unsigned int count = GetSlotCount();
    for (unsigned int i = 0; i < count; ++i)
    {
        SInventorySlot* slot = &m_slots[i];
        if (((slot->m_ref.m_typeTag & (int)slot->m_ref.m_handle) == -1 ||
             !QueryHandleField0x54(&slot->m_ref)) &&
            StoreItemIntoSlot(item, i, notify))
        {
            return 1;
        }
    }
    return 0;
}
