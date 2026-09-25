//============================================================================
//  JassTypeTable's bucket array.  0x6F4499C0 ComputeChunk, 0x6F44E780
//  SetAlloc, 0x6F44F7B0 SetCount.
//============================================================================
#include "jasssymtables.h"
#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@UJASSTYPENODE@@$0?CCCCCCCD@@@"
#include "tsarray.inl"
#include "tslist.inl"

typedef TSExplicitList<JASSTYPENODE> BUCKET;

template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
template void         TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
template void         TSGrowableArray<BUCKET>::SetCount(unsigned int);
template void         TSExplicitList<JASSTYPENODE>::UnlinkAll();
