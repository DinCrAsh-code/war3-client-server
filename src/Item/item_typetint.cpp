//============================================================================
//  0x6F2B8FE0 - QueryItemTypeTint: the per-type tint CItem's own slot 65
//  (item_slot65.cpp) modulates the terrain tint with.
//
//  The same per-item-type record table CItem::Method_0x17C reads
//  (itemtypetable.h, item_typelookup.cpp), keyed the same way and hashed
//  with the same AGILE_TYPE_ID::Hash - a different field of the same record
//  (+0x94 rather than +0x1C) and a different "no record" answer (all-ones
//  rather than zero, because this one is a colour and white is the identity
//  for a modulate).
//
//  A __fastcall free function with the output through ecx, not a member:
//  the shipped code takes the type id in edx and cleans nothing, which a
//  thiscall member never does.
//
//  Own translation unit: TSHashTable<ITEM_TYPE_DATA,AGILE_TYPE_ID>::Ptr and
//  AGILE_TYPE_ID::Hash are both real calls out of it.
//============================================================================
#include "itemtypetable.h"

unsigned int* __fastcall QueryItemTypeTint(unsigned int* out, unsigned int typeId)
{
    //  A named AGILE_TYPE_ID local whose address is taken twice, exactly as
    //  in CAgent::ReleaseSelf (agent_releaseself.cpp) and in
    //  item_typelookup.cpp's own helper.
    AGILE_TYPE_ID id(typeId);
    ITEM_TYPE_DATA* record = g_itemTypeTable.Ptr(id.Hash(), id);

    //  Two separate returns, not one shared exit: the shipped code has two
    //  full epilogues, which is what an early return on the found path
    //  gives and a single `result = ...; return result;` does not.
    if (record != 0)
    {
        *out = record->m_field94;
        return out;
    }
    *out = 0xFFFFFFFF;
    return out;
}
