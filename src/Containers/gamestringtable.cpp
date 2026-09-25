//============================================================================
//  0x6F5C8ED0 - TAllocatorHashTable<STRINGHASHNODE,HASHKEY_STR,256>::Ptr.
//
//  The body moved to Containers/allochash.inl when the file cache's own two
//  instantiations of the same shipped template landed (0x6F4BF040 and
//  0x6F4BF0D0 are this function at HASHKEY_STRI).  The only thing that is
//  per-instantiation is the compare: HASHKEY_STR is Storm ordinal 508, case
//  sensitive, and HASHKEY_STRI is 509.
//
//  Own translation unit: GetGameString calls it twice for real.
//============================================================================
#include "gamestrings.h"
#include "allochash.inl"

template STRINGHASHNODE*
TAllocatorHashTable<STRINGHASHNODE, HASHKEY_STR, 256>::Ptr(const char*);
