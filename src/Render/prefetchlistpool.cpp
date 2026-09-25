//============================================================================
//  0x6F4BCA30 - TSHashTable_P<PrefetchListFile,HASHKEY_STRI_P>::AllocString.
//  Its own translation unit: New (0x6F4C1510) calls it for real.
//============================================================================
#include "filecache.h"
#include "hashtablep.inl"

template char* CPrefetchListTable::AllocString(const char*);
