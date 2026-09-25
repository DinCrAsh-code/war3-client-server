//============================================================================
//  The solid-colour texture table's bucket array - the same
//  TSGrowableArray<TSExplicitList<T> > at a second element type.
//
//  0x6F4DE920 ComputeChunk, 0x6F4E0F90 SetAlloc, 0x6F4E1D70 SetCount.
//============================================================================
#include "texture.h"
#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@UCSolidTextureHash@@$0?CCCCCCCD@@@"
#include "tsarray.inl"

typedef TSExplicitList<CSolidTextureHash> BUCKET;

template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
template void TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
template void TSGrowableArray<BUCKET>::SetCount(unsigned int);
