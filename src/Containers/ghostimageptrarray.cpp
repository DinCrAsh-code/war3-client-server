//============================================================================
//  TSGrowableArray<CGhostImage*> - the world frame's per-kind pointer array
//  of ghost images (the translucent build-mode previews).  0x6F38D510's
//  allocation tag is the RTTI descriptor name `.PAVCGhostImage@@`.
//
//  0x6F38D4B0 ComputeChunk (cap 0x40 = 256/sizeof(T)), 0x6F38D510 SetAlloc.
//============================================================================
class CGhostImage;

#define TSARRAY_TYPENAME ".PAVCGhostImage@@"
#include "tsarray.inl"

template unsigned int TSGrowableArray<CGhostImage*>::ComputeChunk(unsigned int);
template void TSGrowableArray<CGhostImage*>::SetAlloc(unsigned int);
