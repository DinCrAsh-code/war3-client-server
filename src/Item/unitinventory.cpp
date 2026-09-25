//============================================================================
//  0x6F282CA0 / 0x6F282D30 - two thirds of CUnit's inventory gate, the
//  members JASS_UnitAddItemById calls between creating the item and
//  registering its agent.  CanUseInventory, which both of these call, is in
//  unitinventory_canuse.cpp so that it stays a real call.  See
//  unitinventory.h.
//============================================================================
#include "unitinventory.h"

//----------------------------------------------------------------------------
//  0x6F282CA0 - the inventory pointer is loaded once, up front, and used
//  only in the last block; the shipped code keeps it in edi across the
//  CanUseInventory call, which is what the early local here reproduces.
//----------------------------------------------------------------------------
int CUnit::CheckItemAddable(int skipOwnerCheck, int isPowerup, int allowFullBag)
{
    CAbilityInventory* inventory = UnitInventory(this);

    if (!CanUseInventory())
        return kItemAddNoInventory;

    if (UnitFlags(this) & kUnitFlagBusy)
        return kItemAddUnitBusy;

    if (!skipOwnerCheck && !IsOwnedByAPlayer())
        return kItemAddNotOwned;

    if (isPowerup)
    {
        if (!inventory->AcceptsPowerup())
            return kItemAddPowerupDenied;
    }
    else if (inventory->IsFull())
    {
        if (allowFullBag || !(UnitFlags(this) & kUnitFlagAcceptsFullBag))
            return kItemAddInventoryFull;
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F282D30 - the powerup bit is pulled out of the item before the gate
//  runs, because the shipped code keeps it in esi and reuses it to pick the
//  handover call afterwards rather than reading the item twice.
//----------------------------------------------------------------------------
int CUnit::AddItem(void* item, int skipOwnerCheck)
{
    CAbilityInventory* inventory = UnitInventory(this);
    if (!inventory)
        return 0;

    int isPowerup = (ItemFlags(item) >> 11) & 1;

    if (CheckItemAddable(skipOwnerCheck, isPowerup, 0))
        return 0;

    //  The non-powerup case is the fall-through and the powerup case is
    //  the jump target, which is the way round the dump has it: `jnz` past
    //  StoreItem to ApplyPowerup, not the reverse.
    if (!isPowerup)
        return inventory->StoreItem(item, 0);

    inventory->ApplyPowerup(item, 0);
    return 1;
}
