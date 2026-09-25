//============================================================================
//  0x6F42A3C0 - JASS VM/compiler core internals (jassvm-core-A).  `retn 0`,
//  __fastcall(ecx=store, edx=arr) - not a CDataStore member itself, since a
//  thiscall member could not carry `arr` in edx the way every call site
//  here does; it is a free function reached from sub_6F45C170's own JASS
//  argument-array node reader.
//
//  Reads a dword count off `store`, clamped by `arr`'s own current element
//  count (or a 0xFFFF0 default when `arr` is still empty), then reads that
//  many raw pointer-sized dwords straight into `arr`'s own backing storage
//  via TSGrowableArray<void*>::SetCount plus one CDataStore::ReadDword per
//  slot.  When the declared count exceeds the cap, bumps the store's own
//  read position one past its pre-read length instead (a sentinel "went
//  past declared data" mark, not an ordinary one-dword advance) and bails.
//  Returns `store` unconditionally either way, matching every other
//  CDataStore Read* member's stream-style return.
//============================================================================
#include "cdatastore.h"
#include "storm.h"

CDataStore* __fastcall JassReadPointerArray(CDataStore* store, TSGrowableArray<void*>* arr)
{
    unsigned int cap = arr->m_count;
    if (cap == 0)
        cap = 0xFFFF0;

    unsigned int count = 0;
    store->ReadDword(&count);

    unsigned int limit = store->m_field10;

    if ((unsigned int)store->m_readPos > limit)
        return store;

    if (count > cap)
    {
        store->m_readPos = limit + 1;
        return store;
    }

    arr->SetCount(count);

    for (unsigned int i = 0; i < count; i++)
    {
        store->ReadDword((unsigned int*)&arr->m_data[i]);
        if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
            return store;
    }

    return store;
}
