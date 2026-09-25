//============================================================================
//  0x6F2BA0A0 - CItem's vtable slot 95 (+0x17C), where a plain widget
//  answers 0 (0x6F266B50, widget_vtable_leaves.cpp).
//
//  IDA sees this as one function whose body starts with two instructions at
//  0x6F2BA0A0 and continues at `loc_6F2B8AE0` - a tail jump into a block it
//  never promoted to a function of its own - so the dump carries both
//  halves under the one heading and the reconstruction has to produce both.
//  The lookup half is therefore written as a same-file static helper that
//  MSVC inlines, not as its own translation unit.
//
//  What it does: look the item's own footprint type up in the per-type
//  record table (itemtypetable.h) and hand back that record's +0x1C word,
//  or 0 when the type has no record.
//============================================================================
#include "item.h"
#include "itemtypetable.h"

static unsigned int LookupItemTypeWord(unsigned int typeId)
{
    //  A named AGILE_TYPE_ID local whose address is taken twice, exactly as
    //  in CAgent::ReleaseSelf (agent_releaseself.cpp): once as Hash()'s
    //  `this`, once as Ptr()'s key reference.
    AGILE_TYPE_ID id(typeId);
    ITEM_TYPE_DATA* record = g_itemTypeTable.Ptr(id.Hash(), id);
    //  The non-zero answer first: written as an early `return 0` on the
    //  null path, MSVC puts the zero exit in front and the shipped
    //  `jz`-to-the-end order inverts.
    if (record != 0)
        return record->m_field1C;
    return 0;
}

unsigned int CItem::Method_0x17C()
{
    return LookupItemTypeWord(m_footprintType);
}
