//============================================================================
//  0x6F4058D0 - read one cache entry's header: the dword at +0x44 and then
//  whatever the longer form at +0x1C needs.
//============================================================================
#include "cdatastore.h"

struct SGameCacheEntry
{
    char         m_reserved00[0x1C];
    char         m_body[0x28];      // +0x1C
    unsigned int m_word44;          // +0x44
};

//  0x6F405810 - the body's own reader; not in this tree.
CDataStore* __fastcall GameCacheReadEntryBody(CDataStore* store, void* body);

CDataStore* __fastcall GameCacheReadEntryHeader(CDataStore* store,
                                                SGameCacheEntry* entry)
{
    store->ReadDwordAlt(&entry->m_word44);
    GameCacheReadEntryBody(store, entry->m_body);
    return store;
}
