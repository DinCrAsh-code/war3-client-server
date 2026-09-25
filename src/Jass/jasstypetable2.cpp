//============================================================================
//  TAllocatorHashTable<JASSTYPENODE2,HASHKEY_STR,64>'s loop-carrying
//  members: 0x6F45AE80 PtrInterned, 0x6F45B8A0 Initialize, 0x6F45C3E0
//  CheckGrow, 0x6F45BD00 Rehash, 0x6F45C6D0 NewNode - a second table of the
//  same shape as JassTypeTable (jasstypetable.cpp), reached from
//  sub_6F45D300 right after JassTypeTable's own lookup.
//============================================================================
#include "jasssymtables.h"
#include "allochash.inl"

template JASSTYPENODE2* JassTypeTable2::PtrInterned(const char*);
template void           JassTypeTable2::Initialize();
template int            JassTypeTable2::CheckGrow(unsigned int);
template void           JassTypeTable2::Rehash(unsigned int);
template JASSTYPENODE2* JassTypeTable2::NewNode(unsigned int, void*, void*);
