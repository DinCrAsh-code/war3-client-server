//============================================================================
//  0x6F4BD640 - TAllocatorHashTable<PrefetchNode,HASHKEY_STRI,128>::Unlink.
//  Separate from Delete (0x6F4BE220), which calls it for real.
//============================================================================
#include "filecache.h"
#include "allochash.inl"

template void CPrefetchTable::Unlink(PrefetchNode*);
