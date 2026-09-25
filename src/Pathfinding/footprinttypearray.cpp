//============================================================================
//  TSGrowableArray for the footprint type table's bucket array.
//  0x6F30E380 ComputeChunk, 0x6F31E0C0 SetAlloc, 0x6F3234F0 SetCount.
//============================================================================
#include "footprinttype.h"
#include "tsarray.inl"

typedef TSExplicitList<CFootprintTypeData> BUCKET;

template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
template void TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
template void TSGrowableArray<BUCKET>::SetCount(unsigned int);
