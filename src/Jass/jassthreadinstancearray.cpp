//============================================================================
//  0x6F44CA10 - grow JassThreadLocal's own instance-slot array
//  (TSGrowableArray<void*>, jassthreadstate.h) by one and hand back the
//  fresh slot, uninitialised - the generic Containers/tsarray.inl Grow()
//  shape (no placement-construct, unlike New()), reusing the same
//  ComputeChunk/SetAlloc this repo's ptrarray.cpp already instantiates for
//  TSGrowableArray<void*> (0x6F3A0480/0x6F3A1AA0).
//============================================================================
#include "storm.h"
#include "tsarray.inl"

//  0x6F44CA10
template void** TSGrowableArray<void*>::Grow();
