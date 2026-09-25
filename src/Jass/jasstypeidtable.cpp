//============================================================================
//  TAllocatorHashTable<JASSTYPEIDNODE,HASHKEY_STR,64>'s loop-carrying
//  members: 0x6F45B000 PtrHashOnly, 0x6F45B840 Initialize, 0x6F45C340
//  CheckGrow, 0x6F45BB30 Rehash, 0x6F45C630 NewNode.  Reached from
//  sub_6F45D300 keyed on a raw hashval rather than a string - see
//  jasssymtables.h's own note on JASSTYPEIDNODE.
//============================================================================
#include "jasssymtables.h"
#include "allochash.inl"

template JASSTYPEIDNODE* JassTypeIdTable::PtrHashOnly(unsigned int,
                                                       unsigned int const&);
template void            JassTypeIdTable::Initialize();
template int             JassTypeIdTable::CheckGrow(unsigned int);
template void            JassTypeIdTable::Rehash(unsigned int);
template JASSTYPEIDNODE* JassTypeIdTable::NewNode(unsigned int, void*, void*);
