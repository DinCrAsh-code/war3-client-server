//============================================================================
//  The string table's bucket array: 0x6F4C41D0 ComputeChunk, 0x6F4C4F70
//  SetAlloc, 0x6F4C51A0 SetCount.
//============================================================================
#include "cstringrep.h"
#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@VCStringRep@@$0?CCCCCCCD@@@"
#include "tsarray.inl"

typedef TSExplicitList<CStringRep> BUCKET;

template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
template void TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
template void TSGrowableArray<BUCKET>::SetCount(unsigned int);
