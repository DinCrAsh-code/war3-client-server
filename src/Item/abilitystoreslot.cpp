//============================================================================
//  0x6F0F8C10 - CAbilityInventory::StoreItemIntoSlot: the per-slot attempt
//  StoreItem's own loop calls, index already chosen.  Refuses if the index
//  is out of GetSlotCount()'s range or the slot already holds something,
//  applies the item's powerup effect to the recipient, resolves a
//  "cheaper of two price fields" comparison against a row this function
//  searches for itself, resolves the item's own handle into the slot, and
//  finally notifies the recipient (a sound, a player-slot bump, an
//  attachment-count refresh) before returning success.
//
//  Own translation unit: every one of its own callees not already
//  established elsewhere in this batch is a real, out-of-line call with no
//  dump of its own (itemabilitybatch2_thunks.cpp) - StoreItem's own caller
//  loop cannot see any of this inlined, and this function is 174
//  instructions on its own.
//
//  This one is read close to the raw disassembly on purpose rather than
//  restructured into "clean" C++: the row search walks a *second*,
//  disjoint array on `this` (base +0xC0, count at +0xBC) that this call
//  tree's own view of CAbilityInventory never otherwise reaches
//  (unitinventory.h's own view stops at +0x78), the two per-row callbacks
//  are read at fixed byte offsets off an object a slot merely points at
//  (not a conventional vtable slot index), and the price-adoption branch
//  unconditionally overwrites the row's own price-A once the item's price
//  beats it, only conditionally adopting price-B inside that same branch -
//  the shipped code really does nest the second `fcompp` inside the first
//  branch's fallthrough rather than testing both independently.
//============================================================================
#include "unitinventory.h"
#include "itemhandlemain.h"
#include "game.h"
#include "itemtypequeries.h"     // ItemTypeAbilitySlotCategory
#include "unitsubgrouprefresh.h" // SPlayerSubgroupTarget

struct SAttachmentOwnerQuery { void* __thiscall QueryOwner(); };
struct SAttachmentCounterNotify { void __thiscall OnCountChanged(); };

//  sub_6F3A1650 - IndexedArrayHolder_6F3A1650::At, already reconstructed in
//  Misc/misc_field_getters.cpp; redeclared locally the way every other
//  caller of it does (its own header note explains why: one shipped
//  address, one symbol, and every caller spells the identical shape).
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

//  Four of what looked like this function's own out-of-scope callees
//  turned out to already be real reconstructions from the concurrent CUnit
//  inventory-closure batch, at the same addresses - `recipient` here is a
//  `CUnit*` throughout (Unit/unit_subgrouprefreshgate.cpp,
//  Unit/unit_subgrouprefreshnotify.cpp, itemtypequeries.cpp,
//  itemsubgrouprefresh.h all agree), so those four are used directly
//  rather than redeclared under a second, thunked name for the same
//  address (which cost a shared score elsewhere the first time this batch
//  tried it - verify.py's address-to-name canonicalisation only tolerates
//  one funcmap row per address).  The remaining two are still this
//  function's own - see itemabilitybatch2_thunks.cpp.
struct SItemPriceFieldQuery     { void __thiscall QueryFieldA(void* out);
                                   void __thiscall QueryFieldB(void* out); };
struct SItemPriceFieldWriter    { void __thiscall WriteFields(void* out1, void* out2); };
struct SRecipientNotifyA        { void __thiscall NotifyA(); };
struct SRecipientItemNotify     { void __thiscall NotifyItem(void* item); };

//  0x6F2B9600 - see itemapplypowerupeffect.cpp.
struct SItemPowerupSource
{
    int __thiscall ApplyPowerupEffect(void* recipient, int arg4, int arg8,
                                       int argC, int arg10);
};

int __fastcall PlayInterfaceSound(const char* name);

int CAbilityInventory::StoreItemIntoSlot(void* item, int index, int notify)
{
    if ((unsigned int)index >= GetSlotCount())
        return 0;

    if (GetItemInSlot(index))
        return 0;

    void* recipient = m_cachedOwner;
    if (!recipient)
        recipient = ((SAttachmentOwnerQuery*)this)->QueryOwner();

    unsigned int wantsSubgroupRefresh = ((CUnit*)recipient)->WantsSubgroupRefreshNotify();

    int fieldB8 = *(int*)((char*)this + 0xB8);
    ((SItemPowerupSource*)item)->ApplyPowerupEffect(recipient, 0, index, 1, fieldB8);

    unsigned int rowKey = ItemTypeAbilitySlotCategory(*(unsigned int*)((char*)item + 0x30));

    int priceA = g_CFloatZero.m_bits;
    int chosenB = g_CFloatZero.m_bits;

    //  A second, disjoint array on `this` - count at +0xBC, elements at
    //  +0xC0 - searched from the end backwards for a dword equal to
    //  `rowKey`.
    unsigned int rowCount = *(unsigned int*)((char*)this + 0xBC);
    unsigned int* rowsEnd = (unsigned int*)((char*)this + 0xC0) + rowCount;
    unsigned int remaining = rowCount;
    for (;;)
    {
        unsigned int before = remaining;
        --remaining;
        --rowsEnd;
        if (before == 0)
            break;
        if (rowKey == *rowsEnd)
        {
            //  The matched row's own two price callbacks: `this + (i*5+0x37)*4`
            //  holds a pointer to an object whose +0x18/+0x10 are function
            //  pointers, each called with the *slot's own address* as its
            //  `this` and one out-pointer.
            typedef void* (__thiscall *RowProbeFn)(void*, void*);
            char* slotAddr = (char*)this + (unsigned int)(remaining * 5 + 0x37) * 4;
            void* rowObj = *(void**)slotAddr;

            RowProbeFn probeA = *(RowProbeFn*)((char*)rowObj + 0x18);
            priceA = *(int*)probeA(slotAddr, &priceA);

            RowProbeFn probeB = *(RowProbeFn*)((char*)rowObj + 0x10);
            chosenB = *(int*)probeB(slotAddr, &chosenB);
            break;
        }
    }

    float itemPriceA, itemPriceB;
    ((SItemPriceFieldQuery*)item)->QueryFieldA(&itemPriceA);
    ((SItemPriceFieldQuery*)item)->QueryFieldB(&itemPriceB);

    if (itemPriceA > *(const float*)&priceA)
    {
        priceA = *(int*)&itemPriceA;
        if (itemPriceB > *(const float*)&chosenB)
            chosenB = *(int*)&itemPriceB;
    }

    if (*(const float*)&chosenB != *(const float*)&g_CFloatZero &&
        *(const float*)&priceA != *(const float*)&g_CFloatZero)
    {
        ((SItemPriceFieldWriter*)item)->WriteFields(&priceA, &chosenB);
    }

    unsigned int typeTag = *(unsigned int*)((char*)item + 0x10);
    unsigned int handle  = *(unsigned int*)((char*)item + 0xC);
    SInventorySlot* slot = &m_slots[index];

    CHandleObject* resolved = LookupHandle(handle, typeTag);
    if (resolved && *(unsigned int*)((char*)resolved + 0xC) == 0x2B61676Cu /* 'agl+' */)
    {
        slot->m_ref.m_handle  = *(unsigned int*)((char*)resolved + 0x14);
        slot->m_ref.m_typeTag = *(int*)((char*)resolved + 0x18);
    }
    else
    {
        slot->m_ref.m_typeTag = -1;
        slot->m_ref.m_handle  = (unsigned int)-1;
    }

    IndexedArrayHolder_6F3A1650* world = (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
    if (notify)
    {
        typedef int (__thiscall *QuerySlot25Fn)(void*);
        int actingSlot = *(unsigned short*)((char*)world + 0x28);
        QuerySlot25Fn slot25 = *(QuerySlot25Fn*)(*(void***)recipient + 25);
        if (actingSlot == slot25(recipient))
            PlayInterfaceSound("ItemGet");
    }

    ((SRecipientNotifyA*)recipient)->NotifyA();

    void* recipient2 = m_cachedOwner;
    if (!recipient2)
        recipient2 = ((SAttachmentOwnerQuery*)this)->QueryOwner();

    ((CUnit*)recipient2)->NotifySubgroupChanged();

    ((SAttachmentCounterNotify*)this)->OnCountChanged();

    if (!wantsSubgroupRefresh)
    {
        int actingSlot = *(unsigned short*)((char*)world + 0x28);
        int playerSlotRecord = world->At((unsigned int)actingSlot);
        SPlayerSubgroupTarget* sub = *(SPlayerSubgroupTarget**)((char*)playerSlotRecord + 0x34);
        sub->NotifyAsyncRefreshSubgroups(1);
    }

    if (!(*(unsigned int*)((char*)recipient + 0x5C) & 0x40000000))
        ((SRecipientItemNotify*)recipient)->NotifyItem(item);

    return 1;
}
