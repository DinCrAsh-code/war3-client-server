//============================================================================
//  TSGrowableArray<CDestructable*> - one of the four per-kind pointer arrays
//  the world frame keeps of the objects it has to draw.  The element type is
//  not a guess: 0x6F38D110 hands Storm the RTTI descriptor name
//  `.PAVCDestructable@@` as its allocation tag - `class CDestructable*`.
//
//  0x6F38D0B0 ComputeChunk (cap 0x40 = 256/sizeof(T)), 0x6F38D110 SetAlloc.
//  A pointer element has nothing to construct, copy or destroy beyond its
//  four bytes, so every loop the generic template writes around those
//  operations optimises away and what is left is the
//  SMemReAlloc/SMemAlloc/dword-copy/SMemFree dance.
//============================================================================
class CDestructable;

#define TSARRAY_TYPENAME ".PAVCDestructable@@"
#include "tsarray.inl"

template unsigned int TSGrowableArray<CDestructable*>::ComputeChunk(unsigned int);
template void TSGrowableArray<CDestructable*>::SetAlloc(unsigned int);
