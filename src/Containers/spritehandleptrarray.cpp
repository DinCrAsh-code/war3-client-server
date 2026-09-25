//============================================================================
//  TSGrowableArray<HSprite*> - a growable array of Storm sprite handles.
//  0x6F33F510's allocation tag is the RTTI descriptor name `.PAUHSprite@@`,
//  a pointer to a *struct*, which is what makes the element `HSprite*` and
//  not one of the four `class C...*` arrays next door.
//
//  Only SetAlloc is in this call tree; the instantiation's ComputeChunk is
//  never reached from here, so it is not instantiated.
//============================================================================
struct HSprite;

#define TSARRAY_TYPENAME ".PAUHSprite@@"
#include "tsarray.inl"

template void TSGrowableArray<HSprite*>::SetAlloc(unsigned int);
