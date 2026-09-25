//============================================================================
//  TAllocatorHashTable<UncachableNode,HASHKEY_STRI,32>'s five: 0x6F4BF0D0
//  Ptr, 0x6F4C0250 Initialize, 0x6F4C0CD0 CheckGrow, 0x6F4C0480 Rehash,
//  0x6F4C0F90 NewNode.  Same bodies as the prefetch table's, one
//  instantiation on: the shipped link had no /OPT:ICF, so each really is
//  emitted twice.
//============================================================================
#include "filecache.h"
#include "allochash.inl"

template UncachableNode* CUncachableTable::Ptr(const char*);
template void            CUncachableTable::Initialize();
template int             CUncachableTable::CheckGrow(unsigned int);
template void            CUncachableTable::Rehash(unsigned int);
template UncachableNode* CUncachableTable::NewNode(unsigned int, void*, void*);
