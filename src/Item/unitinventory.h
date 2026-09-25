//============================================================================
//  CUnit's inventory gate - the three non-virtual members
//  JASS_UnitAddItemById (0x6F3C74F0) calls on the unit it resolved, and the
//  inventory ability they hand the item to.
//
//  CUnit derives from CSelectable: user_knowledge.json's own vtable dump
//  gives CUnit 118 slots against CSelectable's 107, every one of
//  CSelectable's appearing at the same index, which is the single-
//  inheritance override pattern selectable.h already argues for
//  CWidget -> CSelectable.  All 118 are declared in unit.h now, checked
//  against a committed `ida_query vtable` answer by check_vtables.py.
//
//  These three sit at 0x6F282xxx, the same module as
//  SItemHandleFieldObject::DispatchUnknown (itemhandledispatch.h), which
//  reads +0x1F8 and dispatches vtable+0xB8 exactly the way a CUnit does -
//  so that "unidentified" object is a CUnit as well.  Renaming it is a
//  separate change and this target never calls it, so it is left alone.
//============================================================================
#ifndef UNITINVENTORY_H
#define UNITINVENTORY_H

#include "unit.h"
#include "itemhandleresolve.h"

//----------------------------------------------------------------------------
//  The ability object at CUnit+0x1F8.  Every member is __thiscall and every
//  `retn <n>` below was read off this target's own dump, which is what
//  fixes the argument counts (thunk_abi_audit.py's own question, asked of
//  ordinary declarations rather than of naked thunks).  All real bodies now
//  - the second batch's own worklist was exactly closing this edge.
//----------------------------------------------------------------------------
class CAgent;
class CFloat;

//----------------------------------------------------------------------------
//  One inventory slot: the same nullable {handle, typeTag} pair
//  QueryHandleField0x54 (0x6F4786B0) takes, plus one more word this call
//  tree never reads.  The stride is fixed by the shipped index
//  arithmetic - `lea eax,[ecx+ecx*2]` then `[esi+eax*4+70h]`, i.e.
//  index * 3 dwords from +0x70 - so twelve bytes, and the third word is a
//  reserved one rather than an invented field.
//----------------------------------------------------------------------------
struct SInventorySlot
{
    SOptionalHandleRef  m_ref;          // +0x00
    int                 m_reserved08;   // +0x08
};

class CAbilityInventory
{
public:
    int AcceptsPowerup();               // 0x6F0E3C50, retn 0
    int IsFull();                       // 0x6F0F0E80, retn 0
    int StoreItem(void* item, int notify);    // 0x6F0FD040, retn 8
    void ApplyPowerup(void* item, int notify); // 0x6F0F0EA0, retn 8

    //  JASS_UnitItemInSlot's own three (abilityinventoryslot.cpp,
    //  abilitylevelfield.cpp).  All __thiscall; the two with an argument
    //  are `retn 4` and `retn 8` respectively.
    CAgent* GetItemInSlot(int index);              // 0x6F0F0F00, retn 4
    unsigned int GetSlotCount();                   // 0x6F0E4050, retn 0
    CFloat* GetLevelField(CFloat* out, int level); // 0x6F02E9E0, retn 8

    //  0x6F0E3DB0 - abilityinventoryslot.cpp.  How many of m_slots[0..
    //  m_maxSlots) still resolve to a live handle through
    //  QueryHandleField0x54.  `retn 0`.  Also the numerator IsFull
    //  (abilityisfull.cpp) compares against GetSlotCount() - same address,
    //  reached from two different callers this batch and the CUnit
    //  inventory-closure batch each landed independently.
    unsigned int CountValidHandles();

    //  0x6F0F0F80 - abilityinventorydispatch.cpp.  Detach `item` from
    //  whichever inventory slot still references it (if any), running the
    //  category-record/drop-timer/notify bookkeeping that goes with it.
    //  `retn 4`.  See that file for the rest.
    int DetachAndNotify(void* item);

    //  Two more of the same shape as GetLevelField, each its own row field
    //  (abilitylevelfieldextra.cpp): +0x2C for the unit-ownership probe,
    //  +0x28 for the powerup-acceptance one.
    CFloat* GetOwnershipLevelField(CFloat* out, int level);  // 0x6F02EB30, retn 8
    CFloat* GetPowerupLevelField(CFloat* out, int level);    // 0x6F02EAC0, retn 8

    //  StoreItem's own per-slot attempt (abilitystoreitem.cpp) - a real,
    //  out-of-line call in the shipped code and its own translation unit
    //  for the same reason.
    int StoreItemIntoSlot(void* item, int index, int notify);  // 0x6F0F8C10, retn 0Ch

    //------------------------------------------------------------------
    //  Only the words the reconstructed members above touch are named;
    //  everything else is a gap.  CAbilityInventory's real base (CAbility)
    //  is well past +0x50, and nothing here needs it modelled - reaching
    //  these by offset is what CLAUDE.md's "only touched members get
    //  names" rule asks for.
    //------------------------------------------------------------------
    char            m_reserved00[0x30];
    //  0x30 - a cached "owner" pointer, resolved the slow way through
    //  SAttachmentOwnerQuery::QueryOwner (attachmentownerquery.cpp) once
    //  it is null - ability.h's own CAbility (the fuller view of this same
    //  object) names the identical offset m_field30 for the identical
    //  reason.  Every reader in this call tree (DetachAndNotify,
    //  ApplyPowerup, StoreItemIntoSlot) only *reads* it, falling back to a
    //  fresh QueryOwner() when it is still null rather than writing the
    //  result back here - the shipped code never stores through this
    //  member either, so whatever refreshes the cache lives outside this
    //  call tree.  Same overlapping-view relationship attachmentownerquery.cpp's
    //  own handle/typeTag pair has with this class's own +0x0C/+0x10 (never
    //  named here since nothing in this call tree reads them directly on
    //  this type) - QueryOwner() is called with `this` reinterpreted as
    //  an `SAttachmentOwnerQuery*`, not as a separate object.
    void*           m_cachedOwner;
    //  0x34 - the ability's own type rawcode, the key GetLevelField looks
    //  its record up by.
    unsigned int    m_typeId;
    char            m_reserved38[0x3C - 0x38];
    //  0x3C - CUnit::IsOwnedByAPlayer's own guard (unitisownedbyaplayer.cpp):
    //  a positive value here refuses outright, before the level-field probe
    //  even runs.
    int             m_ownershipCheckDisabled;
    char            m_reserved40[0x50 - 0x40];
    //  0x50 - the current level, and GetLevelField's row index.  Negative
    //  is a real, tested case there: it answers zero rather than indexing.
    int             m_level;
    //  0x54 - the type record itself, resolved from m_typeId on first use
    //  and cached here.  Never cleared by anything in this call tree.
    void*           m_pTypeRecord;
    char            m_reserved58[0x14];
    //  0x6C - the hard cap on the slot count, whatever the level says.
    unsigned int    m_maxSlots;
    //  0x70 - the slot array, inline.
    SInventorySlot  m_slots[1];
};

//  CUnit itself is unit.h's, and the four members this header used to
//  declare on a minimal stand-in are declared there now.  That stand-in
//  existed because nothing had modelled CUnit's 118 vtable slots yet and a
//  partial vtable would have been worse than none; the sweep in
//  docs/targets/CUnit__vtable.md closed that, so there is one class again.
//
//  The refusal codes CheckItemAddable hands back.  Only the numbers are
//  attested; each name says which branch produced it.
const int kItemAddNoInventory   = 0x12;
const int kItemAddUnitBusy      = 0x98;
const int kItemAddNotOwned      = 0xD4;
const int kItemAddPowerupDenied = 0xD6;
const int kItemAddInventoryFull = 0x11;

//  The flag bits this gate reads, all in the widget flag word at +0x5C that
//  item.h already names m_field5C on CItem.
const unsigned int kUnitFlagInventoryForced = 0x40000000;
const unsigned int kUnitFlagNoInventory     = 0x80000000;
const unsigned int kUnitFlagBusy            = 0x00000100;
const unsigned int kUnitFlagAcceptsFullBag  = 0x00000004;

//  CUnit's own fields, reached by offset: CSelectable's declared layout
//  stops well short of both and widening it would move every class that
//  derives from it.
inline unsigned int& UnitFlags(const void* unit)
{
    return *(unsigned int*)((char*)unit + 0x5C);
}
inline CAbilityInventory* UnitInventory(const void* unit)
{
    return *(CAbilityInventory**)((char*)unit + 0x1F8);
}

//  The item flag word at +0x20 - the same dword itemhandletable.h reads as
//  SItemHandleObject::m_pendingFlag.  Bit 11 marks a powerup, bit 8 is what
//  UnitAddItemById stamps before handing the item over.
const unsigned int kItemFlagPowerup = 0x800;
const unsigned int kItemFlagGiven   = 0x100;

inline unsigned int& ItemFlags(const void* item)
{
    return *(unsigned int*)((char*)item + 0x20);
}

#endif
