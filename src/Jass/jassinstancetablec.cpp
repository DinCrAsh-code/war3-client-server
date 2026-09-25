//============================================================================
//  A second, self-contained internal hash table from the same JASS instance
//  constructor closure as jassinstancetableb.cpp - see that file's header
//  for what is and is not known about these tables' real element types.
//  This one's seven members are all this session's own (NewNode/Initialize/
//  CheckGrow/Rehash plus the three T-independent bucket-array helpers).
//============================================================================
#include "storm.h"
#include "tsarray.inl"
#include "tshash.inl"
#include "tslist.inl"

struct JassInstanceSlotC : public TSHashObject<JassInstanceSlotC, HASHKEY_NONE>
{
};

typedef JassInstanceSlotC NODE;
typedef TSExplicitList<NODE> BUCKET;

//  0x6F449960
template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
//  0x6F44E640
template void TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
//  0x6F44F6C0
template void TSGrowableArray<BUCKET>::SetCount(unsigned int);
//  0x6F451E90
template void TSHashTable<NODE, HASHKEY_NONE>::Initialize();
//  0x6F453750
template int TSHashTable<NODE, HASHKEY_NONE>::CheckGrow(unsigned int);
//  0x6F452180
template void TSHashTable<NODE, HASHKEY_NONE>::Rehash(unsigned int);
//  0x6F4541B0
template NODE* TSHashTable<NODE, HASHKEY_NONE>::NewNode(unsigned int, void*, void*);

//  UnlinkAll() this table's own Initialize/Rehash call (through
//  SetLinkOffset(), a Storm/storm.h inline) - T-independent code with no
//  address of its own in this closure.
template void TSExplicitList<NODE>::UnlinkAll();
