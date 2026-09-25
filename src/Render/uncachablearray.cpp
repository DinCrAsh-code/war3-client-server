//============================================================================
//  The uncachable table's bucket array: 0x6F4BCDF0 ComputeChunk,
//  0x6F4BE760 SetAlloc, 0x6F4BF8B0 SetCount.
//============================================================================
#include "filecache.h"
#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@UUncachableNode@@$0?CCCCCCCD@@@"
#include "tsarray.inl"

typedef TSExplicitList<UncachableNode> BUCKET;

template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
template void TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
template void TSGrowableArray<BUCKET>::SetCount(unsigned int);
