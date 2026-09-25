//============================================================================
//  TSGrowableArray<CFogMask*> - CFogMask's own array of child/sub-mask
//  pointers (createunit-G4 cluster A, createunitG4_clusterA_fogmask
//  calltree dump). Like Containers/ptrarray.cpp's TSGrowableArray<void*>,
//  a pointer element has nothing for the generic template's per-element
//  construct/destroy loops to do, so what is left is the plain
//  SMemReAlloc/SMemAlloc/SMemFree dance with a dword-at-a-time copy for any
//  moved elements. CFogMask itself is opaque here - only its address is
//  ever touched by this array.
//
//  0x6F407350 ComputeChunk (cap 0x40 = 256/sizeof(CFogMask*)),
//  0x6F4073B0 SetAlloc.
//============================================================================
class CFogMask;

#define TSARRAY_TYPENAME ".PAVCFogMask@@"
#include "tsarray.inl"

template unsigned int TSGrowableArray<CFogMask*>::ComputeChunk(unsigned int);
template void TSGrowableArray<CFogMask*>::SetAlloc(unsigned int);
