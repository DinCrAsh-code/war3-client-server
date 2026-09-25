//============================================================================
//  TSGrowableArray<Span> - one of CFogMask's own growable arrays (createunit
//  -G4 cluster A). Span is 8 bytes; see fogmasktypes.h for why its members
//  stay unnamed. A POD element again means the generic template's per-
//  element construct/destroy loops are no-ops and SetAlloc reduces to the
//  plain SMemReAlloc/SMemAlloc/SMemFree dance with an 8-byte-at-a-time copy
//  for any moved elements.
//
//  0x6F407A30 SetAlloc.
//============================================================================
#include "fogmasktypes.h"

#define TSARRAY_TYPENAME ".?AUSpan@@"
#include "tsarray.inl"

template void TSGrowableArray<Span>::SetAlloc(unsigned int);
