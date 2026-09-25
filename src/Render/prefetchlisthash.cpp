//============================================================================
//  TSHashTable_P<PrefetchListFile,HASHKEY_STRI_P>'s five: 0x6F4BF160 Ptr,
//  0x6F4C0650 Initialize, 0x6F4C0D70 CheckGrow, 0x6F4C06B0 Rehash,
//  0x6F4C1030 NewNode.
//============================================================================
#include "filecache.h"
#include "hashtablep.inl"

template PrefetchListFile* CPrefetchListTable::Ptr(const char*);
template void              CPrefetchListTable::Initialize();
template int               CPrefetchListTable::CheckGrow(unsigned int);
template void              CPrefetchListTable::Rehash(unsigned int);
template PrefetchListFile* CPrefetchListTable::NewNode(unsigned int, void*,
                                                       void*);
