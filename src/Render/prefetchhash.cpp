//============================================================================
//  TAllocatorHashTable<PrefetchNode,HASHKEY_STRI,128>'s five loop-carrying
//  members: 0x6F4BF040 Ptr, 0x6F4C01F0 Initialize, 0x6F4C0C30 CheckGrow,
//  0x6F4C02B0 Rehash, 0x6F4C0EF0 NewNode.
//
//  One translation unit for all five because none of them can inline into
//  another - MSVC declines a body with a loop at /O2 - and the shipped code
//  has a real call at every one of the four edges between them.  The three
//  members that *would* inline (LinkNode, SetKey, Unlink, Delete) each get a
//  file of their own.
//============================================================================
#include "filecache.h"
#include "allochash.inl"

template PrefetchNode* CPrefetchTable::Ptr(const char*);
template void          CPrefetchTable::Initialize();
template int           CPrefetchTable::CheckGrow(unsigned int);
template void          CPrefetchTable::Rehash(unsigned int);
template PrefetchNode* CPrefetchTable::NewNode(unsigned int, void*, void*);
