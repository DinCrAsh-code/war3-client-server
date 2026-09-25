//============================================================================
//  TSGrowableArray<NodeMapDep> - the third of CFogMask's own growable
//  arrays (createunit-G4 cluster A). NodeMapDep is 8 bytes; see
//  fogmasktypes.h for why its members stay unnamed. A POD element means
//  the generic template's per-element construct/destroy loops are no-ops,
//  so SetAlloc reduces to the plain SMemReAlloc/SMemAlloc/SMemFree dance
//  with an 8-byte-at-a-time copy for any moved elements - the identical
//  shape fogspanarray.cpp's Span instantiation has, just a distinct
//  address because it carries its own type tag through Storm's allocator.
//
//  0x6F407C50 SetAlloc.
//============================================================================
#include "fogmasktypes.h"

#define TSARRAY_TYPENAME ".?AUNodeMapDep@@"
#include "tsarray.inl"

template void TSGrowableArray<NodeMapDep>::SetAlloc(unsigned int);
