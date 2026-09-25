//============================================================================
//  TSGrowableArray<FogNode> - another of CFogMask's own growable arrays
//  (createunit-G4 cluster A). FogNode is 20 bytes; see fogmasktypes.h for
//  why its members stay unnamed. A POD element means the generic
//  template's per-element construct/destroy loops are no-ops: SetAlloc
//  reduces to the plain SMemReAlloc/SMemAlloc/SMemFree dance and Assign to
//  ReleaseAndReserve followed by a raw element-sized copy, both with a
//  20-byte-at-a-time move/copy for the elements actually touched.
//
//  0x6F407AC0 SetAlloc, 0x6F407B60 Assign.
//============================================================================
#include "fogmasktypes.h"

#define TSARRAY_TYPENAME ".?AUFogNode@@"
#include "tsarray.inl"

template void TSGrowableArray<FogNode>::SetAlloc(unsigned int);
template void TSGrowableArray<FogNode>::Assign(unsigned int, const FogNode*);
