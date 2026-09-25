//============================================================================
//  See itemhandledispatch.h.
//============================================================================
#include "itemhandledispatch.h"
#include "unitinventory.h"
#include "item.h"

//----------------------------------------------------------------------------
//  0x6F282E40 - `this` (in ecx) is really a CUnit, the same identification
//  unitinventory.h's own header comment already made ("these three sit at
//  0x6F282xxx, the same module as SItemHandleFieldObject::DispatchUnknown
//  ... which reads +0x1F8 and dispatches vtable+0xB8 exactly the way a
//  CUnit does"). `arg` is the CItem whose world position is being
//  published - both callers (item_remove.cpp's CItem::RemoveFromWorld,
//  jasssetitemposition_native.cpp's JASS_SetItemPosition) pass the item
//  they are about to hand to JASS_SetItemPosition_callee1 as `this` there.
//
//  Body: if this unit has no ability-inventory object at +0x1F8, refuse
//  (0).  Otherwise resolve this unit's own path handle (vtable slot 46,
//  GetHandleObject - already established on CUnit) to a world position
//  (CPathRef::SmartPtrToWorldVec3, already established), ask the
//  inventory object to detach `arg` from whatever ability slot still
//  references it and to run its own drop-timer/notify bookkeeping
//  (CAbilityInventory::DetachAndNotify, abilityinventorydispatch.cpp), and
//  only when that reports something was actually found (1) publish the
//  just-resolved position through CItem::JASS_SetItemPosition_callee1
//  with the fixed flag -1.
//
//  The buffer the position lands in (var_C/var_8/var_4, renamed
//  var_18/var_14/var_10 once the epilogue's own `push edi` shifts the
//  frame) is the same local `CWorldVec3` GetHandleObject's own vtable call
//  fills via its `&buffer` argument: `lea eax, [buffer]` / `push eax` is
//  scheduled *ahead* of the vtable call itself, because slot 46 takes no
//  explicit stack argument of its own - the buffer is really
//  SmartPtrToWorldVec3's hidden return pointer, pushed early while eax is
//  free and left on the stack, untouched, across the intervening
//  zero-argument vtable call.
//----------------------------------------------------------------------------
int SItemHandleFieldObject::DispatchUnknown(void* arg)
{
    CUnit* unit = (CUnit*)this;

    CAbilityInventory* inventory = UnitInventory(unit);
    if (!inventory)
        return 0;

    CWorldVec3 pos = ((CPathRef*)unit->GetHandleObject())->SmartPtrToWorldVec3();

    if (!inventory->DetachAndNotify((CItem*)arg))
        return 0;

    ((CItem*)arg)->JASS_SetItemPosition_callee1(&pos.m_x, &pos.m_y, -1);
    return 1;
}
