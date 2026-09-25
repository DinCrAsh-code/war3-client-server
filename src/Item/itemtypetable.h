//============================================================================
//  The per-item-type record table CItem's vtable slot 95 (0x6F2BA0A0,
//  item_typelookup.cpp) reads, keyed by the same AGILE_TYPE_ID FourCC and
//  hashed with the same AGILE_TYPE_ID::Hash as every other registry in this
//  binary (agiletype.h, footprinttype.h).
//
//  Its node has *no* vftable, so TSHashObject's fields sit at +0x00/+0x14
//  rather than +0x04/+0x18 - the same shape AGILE_TYPE_RELATIONSHIP has and
//  the opposite of AGILE_TYPE_DATA's, which is why 0x6F2B88A0 is yet
//  another distinct instantiation of TSHashTable<T,K>::Ptr rather than a
//  shared one.  Only the one field slot 95 reads is named.
//============================================================================
#ifndef ITEMTYPETABLE_H
#define ITEMTYPETABLE_H

#include "agiletype.h"
#include "CFloat.h"
#include "handleobject.h"

struct ITEM_TYPE_DATA : public TSHashObject<ITEM_TYPE_DATA, AGILE_TYPE_ID>
{
    //  +0x18 is a real field this call tree never reads - the node's own
    //  key ends at +0x18 (TSHashObject with a 4-byte K, storm.h) and the
    //  word slot 95 wants is one further in at +0x1C.
    unsigned int m_reserved18;  // 0x18
    unsigned int m_field1C;     // 0x1C
    char         m_reserved20[0x4C - 0x20];
    //  Three adjacent per-type predicates the JASS natives publish, and the
    //  only thing in this repo that says what any of them means: the
    //  registration table binds 0x6F3B2DD0 "IsItemIdPowerup", 0x6F3B2DE0
    //  "IsItemIdSellable" and 0x6F3B2DF0 "IsItemIdPawnable", and each of
    //  those is two instructions into the probe that reads one of these
    //  three words.  Ints and not flags: the probe hands the whole word
    //  back and the native returns it unnarrowed.
    int          m_isPowerup;   // 0x4C
    int          m_isSellable;  // 0x50
    int          m_isPawnable;  // 0x54
    //  0x58-0x64 - four more per-type ints, read one apiece by
    //  CreateItemAtPosition's own populate step (item_typefields.cpp) and
    //  copied straight into the SUnitCreateParams block it builds.  Nothing
    //  in this call tree reads them back, so only the offset is named.
    int          m_field58;     // 0x58
    int          m_field5C;     // 0x5C
    int          m_field60;     // 0x60
    int          m_field64;     // 0x64
    char         m_reserved68[0x6C - 0x68];
    int          m_field6C;     // 0x6C
    //  0x70 - read by CAbilityInventory's own ability-slot dispatcher
    //  (abilityinventorydispatch.cpp) and compared against a small,
    //  per-inventory table of "categories" to find which ability slot (if
    //  any) an item of this type occupies.  Not otherwise identified -
    //  the field keeps its offset rather than a guessed name.
    unsigned int m_abilitySlotCategory;  // 0x70
    char         m_reserved74[0x84 - 0x74];
    //  0x84/0x98 - two more sub-fields the populate step reads by
    //  *address* rather than by value (item_typefields.cpp's
    //  ItemTypeHandleField84/98): each falls back to the shared default
    //  object off_6FA73DF8 when there is no type record, and each result is
    //  fed through GetHandleOrZeroAlias before use - handleobject.h's own
    //  SHandleHolder is exactly what that function takes, and 0xC bytes each
    //  is what leaves no gap before m_field90 and none before m_fieldA4.
    SHandleHolder m_handle84;    // 0x84, 0xC bytes
    float         m_field90;     // 0x90
    //  0x94 - the type's own packed tint, read by QueryItemTypeTint
    //  (item_typetint.cpp) and modulated with the terrain tint under the
    //  item.  A dword here rather than an SPackedColor because this
    //  function only ever copies it whole; the caller does the per-channel
    //  work (packedcolor.h).
    unsigned int  m_field94;     // 0x94
    SHandleHolder m_handle98;    // 0x98, 0xC bytes
    //  0xA4 - a per-type CFloat, read through the fourCC-keyed accessor
    //  (ItemTypeFieldA4) rather than through a resolved ITEM_TYPE_DATA*
    //  the way every other field above is - CreateItemAtPosition looks it
    //  up a second time with the type's own id already in hand instead of
    //  reusing the pointer it already resolved once this call, which is
    //  what the shipped code does (item_typefields.cpp).
    CFloat       m_fieldA4;     // 0xA4
};

class ITEM_TYPE_TABLE : public TSHashTable<ITEM_TYPE_DATA, AGILE_TYPE_ID>
{
public:
    virtual ~ITEM_TYPE_TABLE();
    virtual ITEM_TYPE_DATA* AllocNode(TSExplicitList<ITEM_TYPE_DATA>* bucket,
                                      void* arg1, void* arg2);
};

//  Inline in the data segment, never a pointer to one allocated elsewhere -
//  the same shape footprinttype.h's g_footprintTypeTable has, and the same
//  evidence: `mov ecx, offset dword_6FAB4BCC` loads the address of the
//  symbol itself as `this`.
extern ITEM_TYPE_TABLE g_itemTypeTable;   // dword_6FAB4BCC

//  off_6FA73DF8 - the shared default SHandleHolder every unresolved
//  m_handle84/m_handle98 probe falls back to (item_typefields.cpp).
extern SHandleHolder g_defaultHandleHolder;

#endif
