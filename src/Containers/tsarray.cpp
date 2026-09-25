//============================================================================
//  TSGrowableArray for the relationship table's bucket array - an array of
//  TSExplicitList, which is why every element needs constructing, destroying
//  and relinking by hand rather than memcpy'd.
//
//  0x6F467B00 ComputeChunk, 0x6F46DFB0 SetAlloc, 0x6F46F0C0 SetCount.
//============================================================================
#include "agiletype.h"
#include "tsarray.inl"

typedef AGILE_TYPE_DATA::AGILE_TYPE_RELATIONSHIP REL;
typedef TSExplicitList<REL> BUCKET;

template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
template void TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
template void TSGrowableArray<BUCKET>::SetCount(unsigned int);
