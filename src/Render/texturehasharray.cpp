//============================================================================
//  The texture table's bucket array - TSGrowableArray of TSExplicitList, so
//  every element has to be constructed, destroyed and relinked by hand.
//
//  0x6F4DE8C0 ComputeChunk, 0x6F4E0E50 SetAlloc, 0x6F4E1C80 SetCount.
//============================================================================
#include "texture.h"
#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@UCTextureHash@@$0?CCCCCCCD@@@"
#include "tsarray.inl"

typedef TSExplicitList<CTextureHash> BUCKET;

template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
template void TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
template void TSGrowableArray<BUCKET>::SetCount(unsigned int);
