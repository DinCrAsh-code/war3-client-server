//============================================================================
//  0x6F4BE220 - TAllocatorHashTable<PrefetchNode,HASHKEY_STRI,128>::Delete.
//============================================================================
#include "filecache.h"
#include "allochash.inl"

template PrefetchNode* CPrefetchTable::Delete(PrefetchNode*);
