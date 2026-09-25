//============================================================================
//  0x6F3C74F0 - JASS_UnitAddItemById, the native's own top-level entry.
//
//  Its registration is the ground truth for the signature: the JASS native
//  table at 0x6F3D4020 registers it as
//
//      push offset "(Hunit;I)Hitem;"
//      mov  edx, offset "UnitAddItemById"
//      mov  ecx, offset sub_6F3C74F0
//      call sub_6F455110
//
//  i.e. (unit handle, integer) -> item handle, and the bare `retn` makes it
//  __cdecl, the same shape JASS_SetItemPosition already has.
//
//  What it does: resolve the unit, ask it where it is, create an item of
//  the requested type on the ground at that position, and - only if the
//  unit can actually take it - move it into the unit's inventory.  The
//  return value is the *agent slot token* the item registers under, which
//  is what a JASS `item` handle is; every refusal path returns 0, which is
//  JASS `null`.
//
//  Two shapes here are load-bearing and should not be "cleaned up":
//
//   * `position` is copied out of the value SmartPtrToWorldVec3 returns
//     rather than used in place.  The shipped code materialises the return
//     into one 12-byte temporary and then copies all three words into a
//     second one whose address it takes - two CWorldVec3s, 0x18 bytes of
//     frame - so the copy is in the source, not something the compiler
//     invented.  Fold it away and the temporary disappears with it.
//
//   * the two arguments to CreateItemAtPosition are `&position.m_x` and
//     `&position.m_y`, two separate CFloat pointers into the same object,
//     not one vector pointer.  That is the JASS-native convention for
//     reals throughout this module (JASS_SetItemPosition takes its X and Y
//     the same way) and it is what 0x6F2B9940 reads: it dereferences edx
//     for one float and its stack argument for the other, then stores them
//     side by side.  m_z is stored by the copy and never read.
//============================================================================
#include "unitinventory.h"
#include "game.h"
#include "itemhandlemain.h"
#include "agentregistry.h"

//  0x6F3BDCB0 - the unit-handle twin of ResolveItemHandle: the same lazy
//  singleton, the same 'agl+' tag and pending-flag guards, under its own
//  SEH frame because it holds two ref-counted smart pointers while it
//  works.  Not reconstructed here.
CUnit* __fastcall ResolveUnitHandle(int hUnit);

//  0x6F2B9940 - create an item of type `itemId` on the ground at (*x, *y)
//  and return it, or null.  __fastcall: the type in ecx, the X pointer in
//  edx, the Y pointer on the stack (`retn 4`).
void* __fastcall CreateItemAtPosition(int itemId, const CFloat* x, const CFloat* y);

int __cdecl JASS_UnitAddItemById(int hUnit, int itemId)
{
    CUnit* unit = ResolveUnitHandle(hUnit);
    if (!unit)
        return 0;

    //  Slot 46 (+0xB8) on the widget vtable - CWidget::GetHandleObject,
    //  already established by this repo at the same numeric slot.  What it
    //  hands back is the unit's own path handle, whose value-returning
    //  conversion is CPathRef::SmartPtrToWorldVec3 (CPathRef.cpp).
    CWorldVec3 returned = ((CPathRef*)unit->GetHandleObject())->SmartPtrToWorldVec3();
    CWorldVec3 position(returned);

    void* item = CreateItemAtPosition(itemId, &position.m_x, &position.m_y);

    //  One conjunction rather than three early returns: the shipped code
    //  jumps all three refusals forward to a single shared `xor eax,eax`
    //  tail at the very end of the function, which is the block layout
    //  MSVC picks for a guarded success path and not the one it picks for
    //  `if (...) return 0;` written three times (that hoists the zero
    //  return into the middle of the body and inverts the last branch).
    if (item && unit->CanUseInventory() &&
        !unit->CheckItemAddable(1, (ItemFlags(item) >> 11) & 1, 0))
    {
        ItemFlags(item) |= kItemFlagGiven;
        unit->AddItem(item, 1);

        //  The holder is named before the call rather than used in place:
        //  the shipped code issues GetSlotTable *before* pushing
        //  Register's own two arguments, which is the order a named
        //  receiver forces and the order an in-place call does not (MSVC
        //  evaluates arguments right-to-left and the object expression
        //  last).
        //
        //  That holder and CAgentRegistry are the same object:
        //  itemhandletable.h's m_records at +0x19C is the data pointer of
        //  agentregistry.h's m_slots at +0x194, and both are reached off
        //  dword_6FAB65F4.  The cast says so rather than duplicating the
        //  type.
        CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
        return (int)registry->Register((CUnitAgent*)item, 0);
    }

    return 0;
}
