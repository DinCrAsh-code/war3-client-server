//============================================================================
//  TSGrowableArray<CUnit*> - the collected-candidate array a query record
//  carries at +0x80 (agenteventbinding.h) and reserves 0x100 entries of in
//  its own constructor (src/Agent/unitquerypool.cpp).
//
//  The element type is not a guess: 0x6F0861E0 hands Storm the RTTI
//  descriptor name `.PAVCUnit@@` as its allocation tag - `class CUnit*`.
//
//  0x6F086180 ComputeChunk (cap 0x40 = 256/sizeof(T)), 0x6F0861E0 SetAlloc.
//  Nothing in this call tree reaches this instantiation's Grow(), so it is
//  not instantiated here.
//============================================================================
class CUnit;

#define TSARRAY_TYPENAME ".PAVCUnit@@"
#include "tsarray.inl"

template unsigned int TSGrowableArray<CUnit*>::ComputeChunk(unsigned int);
template void TSGrowableArray<CUnit*>::SetAlloc(unsigned int);
