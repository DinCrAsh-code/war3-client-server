//============================================================================
//  TSGrowableArray<SUnitQuery*> - the pool of query records MakeUnitQuery
//  hands out of (src/Agent/unitquerypool.cpp).  Its storage is the global
//  array at 0x6FAB4494 and its elements are the records themselves, never
//  copied and never released, which is why every per-element loop the
//  generic template writes optimises away exactly as TSGrowableArray<void*>
//  (ptrarray.cpp) does.
//
//  The element type is not a guess: 0x6F274CA0 hands Storm the RTTI
//  descriptor name `.PAUTEnumData@@` as its allocation tag - `struct
//  TEnumData*`, which is the shipped name for the record this repo calls
//  SUnitQuery (agenteventbinding.h).  The name here stays the established
//  one; renaming the type and the fifteen call sites that use it is not
//  this batch's to do, and the tag is written out literally so the next
//  session does not have to re-derive it.
//
//  0x6F274C40 ComputeChunk (cap 0x40 = 256/sizeof(T)), 0x6F274CA0 SetAlloc,
//  0x6F286700 Grow.  All three in one translation unit for the same reason
//  predarray.cpp holds all three of its own: MSVC declines to inline
//  SetAlloc and ComputeChunk into Grow at /Ob2, so the shipped calls stay
//  real calls.
//============================================================================
#include "agenteventbinding.h"

#define TSARRAY_TYPENAME ".PAUTEnumData@@"
#include "tsarray.inl"

template unsigned int TSGrowableArray<SUnitQuery*>::ComputeChunk(unsigned int);
template void TSGrowableArray<SUnitQuery*>::SetAlloc(unsigned int);
template SUnitQuery** TSGrowableArray<SUnitQuery*>::Grow();
