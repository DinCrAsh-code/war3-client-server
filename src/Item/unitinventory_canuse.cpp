//============================================================================
//  0x6F282C60 - CUnit::CanUseInventory, in its own translation unit.
//
//  Its two callers (CheckItemAddable and, through that, AddItem) call it for
//  real in the shipped code, so it must not be inlined into them; the module
//  split is what stops that, not __declspec(noinline) - the shipping build
//  had no such attribute and adding one would be a lie in the source.
//============================================================================
#include "unitinventory.h"

//----------------------------------------------------------------------------
//  The flag test is one `||` out of a single register load: the shipped code
//  tests bit 30 and takes the body immediately if it is set, and only then
//  falls through to a test of the same register for bit 31.  Bit 31 is
//  spelled as an ordinary mask rather than as `(int)flags >= 0`: MSVC reads
//  a top-bit mask straight off the sign flag (`test eax,eax` / `js`), which
//  is the instruction the dump has, where the signed comparison spells the
//  identical test as `jl`.
//
//  Written as a guarded success path returning 1, rather than as three early
//  `return 0`s, because that is what puts the shared `xor eax,eax` tail at
//  the very end where the dump has it - the same block-layout point
//  JASS_UnitAddItemById's own body makes.
//----------------------------------------------------------------------------
int CUnit::CanUseInventory()
{
    unsigned int flags = UnitFlags(this);

    if ((flags & kUnitFlagInventoryForced) || !(flags & kUnitFlagNoInventory))
    {
        CAbilityInventory* inventory = UnitInventory(this);
        if (inventory)
        {
            //  Slot 154 (+0x268) on the ability's own vtable.  Its class is
            //  not otherwise identified by this target - no committed
            //  vtable answer names it - so the slot is dispatched by hand
            //  rather than declared, and its one argument is what the call
            //  site pushes.
            typedef int (__thiscall *InventoryEnabledFn)(void*, int);
            if (((InventoryEnabledFn)(*(void***)inventory)[0x268 / 4])(inventory, 0))
                return 1;
        }
    }

    return 0;
}
