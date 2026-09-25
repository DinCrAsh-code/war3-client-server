//============================================================================
//  0x6F4C0E10 - TAllocatorHashTable<PrefetchNode,HASHKEY_STRI,128>::LinkNode.
//
//  Its own translation unit: SetKey (0x6F4C1210) calls it for real, and at
//  85 instructions with no loop in it /Ob2 would fold it into that one
//  caller if both bodies were visible at once.
//============================================================================
#include "filecache.h"
#include "allochash.inl"

template void CPrefetchTable::LinkNode(PrefetchNode*, unsigned int);
