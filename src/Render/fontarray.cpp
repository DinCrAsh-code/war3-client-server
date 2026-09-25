//============================================================================
//  The font table's bucket array: 0x6F4CC890 ComputeChunk, 0x6F4CD2D0
//  SetAlloc, 0x6F4CD500 SetCount.
//============================================================================
#include "text.h"
#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@UFONTHASHOBJ@@$0?CCCCCCCD@@@"
#include "tsarray.inl"

typedef TSExplicitList<FONTHASHOBJ> BUCKET;

template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
template void TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
template void TSGrowableArray<BUCKET>::SetCount(unsigned int);
