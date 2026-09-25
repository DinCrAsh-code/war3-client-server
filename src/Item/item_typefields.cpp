//============================================================================
//  0x6F2B9090 / 0x6F2B8D40 / 0x6F2B8E60 / 0x6F2B8E90 / 0x6F2B8EC0 /
//  0x6F2B8EF0 / 0x6F2B8FB0 / 0x6F2B9020 / 0x6F2B8F80 / 0x6F2B9050 -
//  CreateItemAtPosition's own per-type field probes (item_populate.cpp),
//  every one of them the same shape itemtypequeries.cpp already established:
//  build an AGILE_TYPE_ID from the fourCC in ecx, hash it, look it up in
//  g_itemTypeTable, and hand back one field of the record - or a fallback
//  when there is no record.
//
//  Own translation unit: nine of the ten are __fastcall leaves nothing else
//  in this call tree reaches, and address-adjacent (0x6F2B8D40-0x6F2B9090)
//  with itemtypequeries.cpp's own three, which is the same 0x6F2B8xxx
//  module.  None calls another, so there is nothing to protect from
//  inlining here - this is just where they live.
//
//  The **existence** probe (0x6F2B9090) normalises the resolved pointer to
//  0/1 with `neg eax / sbb eax,eax / neg eax`, which is exactly what
//  `ptr != 0` as a plain `int` compiles to, so it is written that way
//  rather than as a hand-rolled bit trick.
//
//  The four plain-int probes (0x58/0x5C/0x60/0x64/0x6C - `m_field6C` is the
//  fifth) fall back to 0 on a miss, matching a default-constructed local
//  the shipped code never stores into first.
//
//  The two **address** probes (0x84/0x98) never dereference the record at
//  all - they return `&type->m_handleNN`, or the shared default instance's
//  address on a miss (`mov eax, offset off_6FA73DF8`), which is what makes
//  the SHandleHolder* declaration itemtypetable.h already carries the right
//  one: nothing here needs to know what is inside it, only its address.
//
//  The float probe (0x90) falls back to `fldz` - a genuine 0.0f literal,
//  not g_CFloatZero (that fallback belongs to the CFloat probe below) - so
//  it is spelled `float`, not `CFloat`.
//
//  The CFloat probe (0xA4, 0x6F2B9050) is the odd one out in every other
//  way: it takes an *out* pointer in ecx and the fourCC in edx rather than
//  the fourCC alone in ecx, and it writes through that pointer and returns
//  it, the same out-param shape abilitylevelfield.cpp's GetLevelField
//  already has - and the same reason: CreateItemAtPosition (item_populate.cpp)
//  needs the field's *address* to feed straight into a copy, not its value
//  in a register.
//============================================================================
#include "itemtypetable.h"

int __fastcall ItemTypeExists(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    return g_itemTypeTable.Ptr(id.Hash(), id) != 0;
}

int __fastcall ItemTypeGetField58(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    ITEM_TYPE_DATA* type = g_itemTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field58 : 0;
}

int __fastcall ItemTypeGetField5C(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    ITEM_TYPE_DATA* type = g_itemTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field5C : 0;
}

int __fastcall ItemTypeGetField60(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    ITEM_TYPE_DATA* type = g_itemTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field60 : 0;
}

int __fastcall ItemTypeGetField64(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    ITEM_TYPE_DATA* type = g_itemTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field64 : 0;
}

int __fastcall ItemTypeGetField6C(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    ITEM_TYPE_DATA* type = g_itemTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field6C : 0;
}

SHandleHolder* __fastcall ItemTypeGetHandleField84(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    ITEM_TYPE_DATA* type = g_itemTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? &type->m_handle84 : &g_defaultHandleHolder;
}

SHandleHolder* __fastcall ItemTypeGetHandleField98(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    ITEM_TYPE_DATA* type = g_itemTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? &type->m_handle98 : &g_defaultHandleHolder;
}

float __fastcall ItemTypeGetField90(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    ITEM_TYPE_DATA* type = g_itemTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        return type->m_field90;
    return 0.0f;
}

CFloat* __fastcall ItemTypeGetFieldA4(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    ITEM_TYPE_DATA* type = g_itemTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
    {
        *out = type->m_fieldA4;
        return out;
    }
    *out = g_CFloatZero;
    return out;
}
