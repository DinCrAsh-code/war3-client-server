//============================================================================
//  TSGrowableArray<UnitQueryPredicateFn>'s out-of-line members - the
//  instantiation BuildAndRegisterEventBinding (agenteventbinding.cpp)
//  grows its two filter arrays through.
//
//  0x6F274D30 is ComputeChunk and 0x6F274D90 is SetAlloc for this element
//  type; neither is in this target's dump (both are below its depth), so
//  neither is scored - they are here so that the fifteen inlined `Add`
//  call sites have something real to call, which is the whole reason the
//  instantiation lives in its own translation unit.  Put it in
//  agenteventbinding.cpp and the compiler would inline both bodies into
//  those fifteen sites instead of calling them, exactly as
//  Containers/ptrarray.cpp's own header note says.
//============================================================================
#include "agenteventbinding.h"
#include "tsarray.inl"

template unsigned int TSGrowableArray<UnitQueryPredicateFn>::ComputeChunk(unsigned int);
template void TSGrowableArray<UnitQueryPredicateFn>::SetAlloc(unsigned int);
template UnitQueryPredicateFn* TSGrowableArray<UnitQueryPredicateFn>::Grow();
