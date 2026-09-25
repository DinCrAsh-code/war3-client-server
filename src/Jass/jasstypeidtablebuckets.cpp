//============================================================================
//  JassTypeIdTable's bucket array.  0x6F4593B0 ComputeChunk, 0x6F45A920
//  SetAlloc, 0x6F45B420 SetCount.
//============================================================================
#include "jasssymtables.h"
#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@UJASSTYPEIDNODE@@$0?CCCCCCCD@@@"
#include "tsarray.inl"
#include "tslist.inl"

typedef TSExplicitList<JASSTYPEIDNODE> BUCKET;

template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
template void         TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
template void         TSGrowableArray<BUCKET>::SetCount(unsigned int);
template void         TSExplicitList<JASSTYPEIDNODE>::UnlinkAll();
