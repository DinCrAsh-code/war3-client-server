//============================================================================
//  TSGrowableArray<CItem*> - the world frame's per-kind pointer array of the
//  items it has to draw.  0x6F38D210 hands Storm the RTTI descriptor name
//  `.PAVCItem@@` as its allocation tag, which is what fixes the element type.
//
//  0x6F38D1B0 ComputeChunk (cap 0x40 = 256/sizeof(T)), 0x6F38D210 SetAlloc.
//  Same trivially-copyable element as its three siblings, so the same body.
//============================================================================
class CItem;

#define TSARRAY_TYPENAME ".PAVCItem@@"
#include "tsarray.inl"

template unsigned int TSGrowableArray<CItem*>::ComputeChunk(unsigned int);
template void TSGrowableArray<CItem*>::SetAlloc(unsigned int);
