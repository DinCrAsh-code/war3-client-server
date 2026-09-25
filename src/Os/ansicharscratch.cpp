//============================================================================
//  dword_6FAD13F4 - the multi-byte side of sub_6F6BF900's wide/multi-byte
//  string-conversion scratch pair (see Os/widecharscratch.cpp for the wide
//  side and the shared context).  TSGrowableArray<char>, growing 256
//  elements at a time.
//============================================================================
#define TSARRAY_TYPENAME "aD_11"
#include "tsarray.inl"

//  0x6F4CE3B0 - ComputeChunk: maxchunk = 256/sizeof(char) = 0x100.
template unsigned int TSGrowableArray<char>::ComputeChunk(unsigned int);
//  0x6F4CE410 - SetAlloc: same shape as the wide side's, one byte at a time.
template void TSGrowableArray<char>::SetAlloc(unsigned int);
