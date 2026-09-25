//============================================================================
//  0x6F0057F0 - CPreferencesWar3::StoreValue: remember one preference's
//  value, making the cache node if this is the first time the id is seen.
//
//  The id *is* the node's hash value - the shipped code stores it into
//  +0x00 straight after New() and never writes +0x14 - which is what makes
//  SPreferenceCache::Find's hash-only comparison correct rather than a
//  missing key test (preferencecachefind.cpp).
//
//  `key` is signed and the range test is a `jl`, so a negative id takes the
//  same rejected path as one past the end; that is CPreferencesWar3::
//  GetIntValue's own reading of the same constant and it is repeated here.
//
//  Own translation unit: Find and New are both real calls.
//============================================================================
#include "preferences.h"

extern char g_preferenceBucket;                 // unk_6FAAE310

int CPreferencesWar3::StoreValue(int key, int value)
{
    if (key >= 0x4F)
        return 0;

    SPreferenceEntry* entry = m_cache.Find(key, &g_preferenceBucket);
    if (entry == 0)
    {
        entry = m_cache.NewNode(key, 0, 0);
        entry->m_hashval = key;
    }

    entry->m_value = value;
    return 1;
}
