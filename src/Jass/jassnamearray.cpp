//============================================================================
//  The name table's parallel "declared in order" array - a plain
//  TSGrowableArray<JASSNAMENODE*>, not the bucket array: JassNameTable's own
//  Declare() (jassdeclare.cpp) files every new node here too, indexed by its
//  declaration slot.  0x6F44BAD0 SetCount, 0x6F44BA80 Grow - the generic
//  four-byte-element members storm.h's own TSGrowableArray<T> already
//  defines in Containers/tsarray.inl; only the instantiation is new.
//  0x6F448E40 (ComputeChunk) and 0x6F448EA0 (SetAlloc) for this same T are
//  outside this batch's worklist and are declared, not defined, by
//  TSGrowableArray's own class body in storm.h.
//============================================================================
#include "jasssymtables.h"
#define TSARRAY_TYPENAME ".?AUJASSNAMENODE@@$$CBPAU0@"
#include "tsarray.inl"

template void            TSGrowableArray<JASSNAMENODE*>::SetCount(unsigned int);
template JASSNAMENODE**  TSGrowableArray<JASSNAMENODE*>::Grow();
