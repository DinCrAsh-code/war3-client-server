//============================================================================
//  The prefetch table's bucket array - TSGrowableArray of TSExplicitList, so
//  every element has to be constructed, destroyed and relinked by hand
//  rather than memcpy'd.
//
//  0x6F4BCD90 ComputeChunk, 0x6F4BE620 SetAlloc, 0x6F4BF7C0 SetCount.
//============================================================================
#include "filecache.h"
#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@UPrefetchNode@@$0?CCCCCCCD@@@"
#include "tsarray.inl"

typedef TSExplicitList<PrefetchNode> BUCKET;

template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
template void TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
template void TSGrowableArray<BUCKET>::SetCount(unsigned int);
