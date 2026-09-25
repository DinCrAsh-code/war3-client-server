//============================================================================
//  0x6F26EC20 - CUnit::GetItemInSlot: hand the inventory-slot question
//  straight to the ability object at CUnit+0x1F8, or answer null when the
//  unit has no inventory ability at all.
//
//  Six instructions, and the last is a *tail jump* into
//  CAbilityInventory::GetItemInSlot (0x6F0F0F00, abilityinventoryslot.cpp)
//  rather than a call - which is what an `if (!p) return 0; return
//  p->F(i);` with a matching signature compiles to under /O2.  The two
//  functions must therefore stay in different translation units: inlined
//  into each other there would be no jump to make.
//
//  `retn 4` on the null path fixes the ABI: one stack argument, `this` in
//  ecx.  The tail jump's own target cleans the same four bytes, which is
//  what makes the tail jump legal in the first place.
//============================================================================
#include "unitinventory.h"
#include "abilityinventoryslot.h"

CAgent* CUnit::GetItemInSlot(int index)
{
    CAbilityInventory* inventory = UnitInventory(this);
    if (!inventory)
        return 0;

    return inventory->GetItemInSlot(index);
}
