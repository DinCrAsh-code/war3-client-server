//============================================================================
//  The prefetch-list table's bucket array: 0x6F4BCEA0 ComputeChunk,
//  0x6F4BE8A0 SetAlloc, 0x6F4BF9A0 SetCount.
//============================================================================
#include "filecache.h"
#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@UPrefetchListFile@@$0?CCCCCCCD@@@"
#include "tsarray.inl"

typedef TSExplicitList<PrefetchListFile> BUCKET;

template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
template void TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
template void TSGrowableArray<BUCKET>::SetCount(unsigned int);
