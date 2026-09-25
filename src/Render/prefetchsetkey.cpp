//============================================================================
//  0x6F4C1210 - TAllocatorHashTable<PrefetchNode,HASHKEY_STRI,128>::SetKey.
//  Its own translation unit for the same reason LinkNode has one: its caller
//  (0x6F4C1300) is a real call in the shipped code.
//============================================================================
#include "filecache.h"
#include "allochash.inl"

template void CPrefetchTable::SetKey(PrefetchNode*, const char*);
