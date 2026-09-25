//============================================================================
//  0x6F25B4E0 - GetOrCreateItemCustomData: fetch the game-data cache's own
//  CItemCustomData slot (+0x34, array index 0x0D), lazily creating it the
//  first time.  Same "read the cache field, tail-jump to the lazy-init
//  helper on the not-yet-created path" shape
//  Item/abilitygenericfieldrecord.cpp's own GetGenericFieldValueTable
//  already uses for +0x40 - this one for +0x34 instead, and its own
//  no-arguments/return-the-object-itself convention.
//
//  Reached from CItemDatabase::SetRecordSet's own per-field registration
//  helper (0x6F25C560, Item/itemdatabase_setrecordset.cpp) - the field
//  pool every RegisterTypedField call resolves through.
//============================================================================
#include "customdatasiblings.h"
#include "gamecontext.h"

CItemCustomData* GetOrCreateItemCustomData()
{
    CGameDataContext* context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    CItemCustomData* obj = context->m_pCache->m_field34;
    if (obj != 0)
        return obj;
    return (CItemCustomData*)LazyInitGameDataCacheField34(0x0D, 0);
}
