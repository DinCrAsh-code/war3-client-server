//============================================================================
//  TSGrowableArray<unsigned int> - a plain array of 4-byte scalars (an index
//  or handle list somewhere in this call tree's neighbourhood).  Like
//  ptrarray.cpp's TSGrowableArray<void*>, every per-element construct/
//  destroy the generic template writes is a no-op for a POD element and
//  optimises away, leaving the plain SMemReAlloc/SMemAlloc/SMemFree dance.
//
//  0x6F02C530 ComputeChunk (cap 0x40 = 256/sizeof(unsigned int)),
//  0x6F02C490 SetAlloc.
//============================================================================
#include "tsarray.inl"

template unsigned int TSGrowableArray<unsigned int>::ComputeChunk(unsigned int);
template void TSGrowableArray<unsigned int>::SetAlloc(unsigned int);
