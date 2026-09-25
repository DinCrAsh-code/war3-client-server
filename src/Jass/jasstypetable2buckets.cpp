//============================================================================
//  JassTypeTable2's bucket array.  0x6F459410 ComputeChunk, 0x6F45AA60
//  SetAlloc, 0x6F45B510 SetCount.
//============================================================================
#include "jasssymtables.h"
#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@UJASSTYPENODE2@@$0?CCCCCCCD@@@"
#include "tsarray.inl"
#include "tslist.inl"

typedef TSExplicitList<JASSTYPENODE2> BUCKET;

template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
template void         TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
template void         TSGrowableArray<BUCKET>::SetCount(unsigned int);
template void         TSExplicitList<JASSTYPENODE2>::UnlinkAll();
