//============================================================================
//  TSGrowableArray<CEffectImage*> - the world frame's per-kind pointer array
//  of effect images.  0x6F38D410's allocation tag is the RTTI descriptor
//  name `.PAVCEffectImage@@`, which is what names the element type.
//
//  0x6F38D3B0 ComputeChunk (cap 0x40 = 256/sizeof(T)), 0x6F38D410 SetAlloc.
//============================================================================
class CEffectImage;

#define TSARRAY_TYPENAME ".PAVCEffectImage@@"
#include "tsarray.inl"

template unsigned int TSGrowableArray<CEffectImage*>::ComputeChunk(unsigned int);
template void TSGrowableArray<CEffectImage*>::SetAlloc(unsigned int);
