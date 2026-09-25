//============================================================================
//  0x6F2B8DD0 / 0x6F2B8E00 / 0x6F2B8E30 - the three per-item-type predicate
//  probes the JASS natives 0x6F3B2DD0, 0x6F3B2DE0 and 0x6F3B2DF0 are each
//  two instructions into.
//
//  One shape, three adjacent field offsets on ITEM_TYPE_DATA
//  (itemtypetable.h):
//
//      0x6F2B8DD0  +0x4C  "is this item type a power-up"
//      0x6F2B8E00  +0x50  "... sellable"
//      0x6F2B8E30  +0x54  "... pawnable"
//
//  The middle one has no `### ... @ 0x...` heading anywhere in asm/,
//  because IDA models 0x6F2B8E00 as a *tail chunk* of the two functions
//  that `jmp` into it (0x6F3B2DE0 and 0x6F3C4FF0) rather than as a function
//  of its own - the shape docs/msvc-vc8-idioms.md calls "a shared tail
//  chunk is not a score you can chase".  It is written out here anyway,
//  because the .mix has to have something for JASS_IsItemIdSellable to
//  `jmp` to, and it deliberately gets *no* funcmap.py row: there is no
//  heading for the row's left column to name, so one would only make
//  gen_rename_from_funcmap.py print a dead end.
//
//  Everything else is unittypequeries.cpp's note, which applies unchanged:
//  the **nested** `Ptr(id.Hash(), id)` spelling (every one of these pushes
//  `&id` before calling Hash()), and the *found* arm written first as a
//  conditional expression, because the shipped `jz` jumps forward onto a
//  trailing `xor eax, eax`.
//
//  Own translation unit: none of the three calls another, and all three
//  have to reach 0x6F2B88A0 (itemtypetable.cpp) with a real call.
//============================================================================
#include "itemtypetable.h"
#include "itemtypequeries.h"

int __fastcall ItemTypeIsPowerup(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    ITEM_TYPE_DATA* type = g_itemTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_isPowerup : 0;
}

int __fastcall ItemTypeIsSellable(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    ITEM_TYPE_DATA* type = g_itemTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_isSellable : 0;
}

int __fastcall ItemTypeIsPawnable(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    ITEM_TYPE_DATA* type = g_itemTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_isPawnable : 0;
}

//----------------------------------------------------------------------------
//  0x6F2B8D70 - a fourth, same-shape probe: CAbilityInventory's own
//  ability-slot dispatcher (abilityinventorydispatch.cpp) asks "which
//  ability-slot category does an item of this footprint type belong to,
//  if any" before searching its own small category table for a match.
//  Same nested `Ptr(id.Hash(), id)` idiom as the three above; the only
//  difference is the "not found" answer (0, not one of the three flag
//  words) and that the AGILE_TYPE_ID local's own address does double duty
//  as both Hash()'s `this` and Ptr()'s key-reference argument, which is
//  why `push`ing it happens only once here despite being read twice - the
//  same "already on the stack from the earlier call" scheduling
//  agent_releaseself.cpp's own helper already uses.
//----------------------------------------------------------------------------
unsigned int __fastcall ItemTypeAbilitySlotCategory(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    ITEM_TYPE_DATA* type = g_itemTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_abilitySlotCategory : 0;
}
