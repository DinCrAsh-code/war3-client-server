//============================================================================
//  0x6F0F0F80 - CAbilityInventory::DetachAndNotify.
//
//  SItemHandleFieldObject::DispatchUnknown's own real body
//  (itemhandledispatch.cpp): given an item that may be sitting in one of
//  this inventory's slots, find it, detach it, run whatever drop-timer
//  and category-record bookkeeping goes with the slot it came out of, and
//  - only if this unit is left with no other valid handles and the owner
//  passes CUnit::WantsSubgroupRefreshNotify's own gate - tell the async
//  refresh-subgroups log path about it (CItem::JASS_SetItemPosition_callee1
//  itself is only ever reached from *this* function's caller, not from
//  here).
//
//  Own translation unit, own module (0x6F0F0Fxx): its own callee
//  CAbilityInventory::GetItemInSlot (0x6F0F0F00, abilityinventoryslot.cpp)
//  is a real out-of-line `call` in the dump, not a tail jump, so the two
//  cannot be folded together.
//
//  What the "category" table at +0xBC/+0xC0/+0xDC actually models is not
//  established past what this call tree touches: a small (7-entry) table
//  pairing a FourCC-ish tag with a two-accessor "field source" record,
//  searched by the item's own ItemTypeAbilitySlotCategory() tag.  Neither
//  accessor's own return-then-deref shape (call it with an output buffer,
//  then read one dword back through whatever pointer it hands back) nor
//  the two fixed offsets it reads them from (+0x10, +0x18) matches any
//  already-established "field walker" shape in this codebase (checked
//  against SLinkedFieldWalker, unit_dispatch.cpp's own generic field
//  readers) closely enough to borrow a name from, so it keeps the raw
//  offsets and a generic "field source" name instead of a guess.
//============================================================================
#include "unitinventory.h"
#include "item.h"
#include "itemtypequeries.h"
#include "unitsubgrouprefresh.h"

//  0x6F2B7E60 - SItemRemoveHelpers::DetachLocally, already established
//  (item_remove.cpp).  Re-declared (not re-defined) here for the same
//  reason as SAgentTimerArm in item_armdroptimer.cpp: the naked body
//  item_remove.cpp already defines is the one this links against.
struct SItemRemoveHelpers
{
    void DetachLocally();
};

//----------------------------------------------------------------------------
//  0x6F472890 - SAttachmentOwnerQuery::QueryOwner, already established
//  (Widget/attachmentownerquery.cpp).  Re-declared locally: `this` here is
//  really a CAbilityInventory reinterpreted the same way the header
//  documents its own +0x30 cache field, not a separate object, so there is
//  nothing to include - just the same declaration, resolving to the same
//  symbol.
//----------------------------------------------------------------------------
struct SAttachmentOwnerQuery
{
    void* __thiscall QueryOwner();
};

//----------------------------------------------------------------------------
//  The per-category record CAbilityInventory carries seven of, back to
//  back with the parallel seven-entry tag array right before it (+0xC0..
//  +0xDC is exactly 7 dwords).  Only the one field either accessor call
//  needs (m_source) is named; the rest of the 20-byte stride is an
//  explicit gap.
//----------------------------------------------------------------------------
struct SAbilityCategoryFieldSource
{
    char m_reserved00[0x10];
    //  +0x10/+0x18 - two thiscall "get a field into *out, then hand back
    //  a pointer to it" accessors, called with `this` = the *record*
    //  (below), not this field-source object itself.
    void* m_slot10;
    char m_reserved14[4];
    void* m_slot18;
};

struct SAbilityCategoryRecord
{
    SAbilityCategoryFieldSource* m_source;   // +0x00
    char m_reserved04[0x10];                  // pads the record to 20 bytes
};

//  The region of CAbilityInventory past what unitinventory.h already
//  models (+0x50..+0x70) - a local overlay rather than a header addition,
//  since this is the only call tree that reaches it and the two arrays'
//  sizes (seven entries each) are read off this dump's own fixed
//  arithmetic rather than any bound stored in the object.
struct CAbilityInventoryCategoryView
{
    char                      m_reserved00[0xBC];
    unsigned int              m_categoryCount;       // +0xBC
    unsigned int              m_categoryTag[7];        // +0xC0
    SAbilityCategoryRecord    m_categoryRecord[7];      // +0xDC
};

typedef CFloat* (__thiscall *CategoryFieldReadFn)(void* self, CFloat* out);

//  0x6F3A1650 - IndexedArrayHolder_6F3A1650::At, already established
//  (unit_ownerworldmask.cpp and neighbours).  Re-declared locally the same
//  way every other reader of it does; not shared through a header, so
//  this stays a local copy (at file scope - a local *class* cannot have
//  an out-of-class member definition, and this one is defined by the
//  naked body elsewhere, not here).
struct IndexedArrayHolder_6F3A1650
{
    char           pad[0x28];
    unsigned short m_index;
    int            At(unsigned int index);
};

int CAbilityInventory::DetachAndNotify(void* itemArg)
{
    CItem* item = (CItem*)itemArg;

    void* owner = m_cachedOwner;
    if (!owner)
        owner = ((SAttachmentOwnerQuery*)this)->QueryOwner();

    CUnit* ownerUnit = (CUnit*)owner;
    int wantsNotify = ownerUnit->WantsSubgroupRefreshNotify();

    unsigned int category = ItemTypeAbilitySlotCategory(item->m_footprintType);

    CAbilityInventoryCategoryView* view = (CAbilityInventoryCategoryView*)this;

    CFloat fieldA;
    CFloat fieldB;
    int found = -1;

    for (int i = (int)view->m_categoryCount - 1; i >= 0; --i)
    {
        if (category == view->m_categoryTag[i])
        {
            found = i;
            break;
        }
    }

    if (found >= 0)
    {
        SAbilityCategoryRecord* record = &view->m_categoryRecord[found];
        SAbilityCategoryFieldSource* source = record->m_source;

        CategoryFieldReadFn readSlot18 = (CategoryFieldReadFn)source->m_slot18;
        fieldA = *readSlot18(record, &fieldA);

        CategoryFieldReadFn readSlot10 = (CategoryFieldReadFn)source->m_slot10;
        fieldB = *readSlot10(record, &fieldB);
    }

    unsigned int slotCount = m_maxSlots;
    int detached = 0;

    for (int i = (int)slotCount - 1; i >= 0; --i)
    {
        if (GetItemInSlot(i) != (CAgent*)item)
            continue;

        ((SItemRemoveHelpers*)item)->DetachLocally();

        //  Both fields' bits are the same IEEE single a CFloat already
        //  stores, but the shipped compare against zero goes through the
        //  hardware FPU directly (`fld`/`fucomp`/`fnstsw`/`test ah,44h`),
        //  the same "field read straight into the FPU rather than through
        //  the software-float path" split docs/msvc-vc8-idioms.md records
        //  elsewhere - so the zero test reads the bits as a native float
        //  rather than going through CFloat's own (undefined) operator!=.
        if (*(float*)&fieldB != 0.0f && *(float*)&fieldA != 0.0f
            && (ItemFlags(item) & 0x200) && (ItemFlags(item) & 0x1000))
        {
            item->ArmDropTimer(&fieldA, &fieldB);
        }

        m_slots[i].m_ref.m_handle = (unsigned int)-1;
        m_slots[i].m_ref.m_typeTag = -1;

        ownerUnit->NotifySubgroupChanged();
        detached = 1;
        break;
    }

    if (wantsNotify)
    {
        extern void* g_unk6FAB65F4;

        if (ownerUnit->CountValidInventoryHandles() == 0)
        {
            IndexedArrayHolder_6F3A1650* holder =
                (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
            char* record = (char*)holder->At(holder->m_index);
            void* target = *(void**)(record + 0x34);

            ((SPlayerSubgroupTarget*)target)->NotifyAsyncRefreshSubgroups(1);
        }
    }

    return detached;
}
