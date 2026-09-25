//============================================================================
//  0x6F3C7730 - JASS_UnitItemInSlot, the native's own top-level entry.
//
//  Signature comes from the same JASS native table (0x6F3D4020) that fixes
//  JASS_UnitAddItemById's, and from the bare `retn`:
//
//      "(Hunit;I)Hitem;"  -  (unit handle, integer) -> item handle, __cdecl
//
//  What it does: resolve the unit, refuse if it has no usable inventory,
//  ask the inventory ability for whatever is parked in slot `index`, and
//  hand back that item's *agent slot token* - which is what a JASS `item`
//  handle is.  Every refusal returns 0, which is JASS `null`.
//
//  It is JASS_UnitAddItemById's read-only twin and shares three of its
//  four calls, so the two spellings deliberately match:
//
//   * the guarded-success shape rather than three separate `return 0;`
//     statements.  All three refusals jump forward to one shared
//     `xor eax,eax` / `pop esi` / `retn` tail at the very end of the body,
//     which is the layout MSVC picks for a nested guard and not the one it
//     picks for early returns written out three times (that hoists the
//     zero return into the middle and inverts the last branch).
//
//   * the registry holder is named before the call rather than used in
//     place, because the shipped code issues GetSlotTable *before* pushing
//     Register's own two arguments - the order a named receiver forces and
//     an in-place call does not, MSVC evaluating arguments right-to-left
//     and the object expression last.
//
//  `item` reuses esi after `unit` is dead, which is what a second local
//  whose lifetime does not overlap the first gets.
//============================================================================
#include "unitinventory.h"
#include "game.h"
#include "itemhandlemain.h"
#include "agentregistry.h"
#include "jassnatives.h"

int __cdecl JASS_UnitItemInSlot(int hUnit, int index)
{
    CUnit* unit = ResolveUnitHandle(hUnit);
    if (unit && unit->CanUseInventory())
    {
        CAgent* item = unit->GetItemInSlot(index);
        if (item)
        {
            CAgentRegistry* registry =
                (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
            return (int)registry->Register((CUnitAgent*)item, 0);
        }
    }

    return 0;
}
