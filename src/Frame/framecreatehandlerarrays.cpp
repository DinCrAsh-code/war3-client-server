//============================================================================
//  The container members the two create-handler registries' NewNode reaches:
//  their bucket arrays' SetCount and their bucket lists' UnlinkAll.
//
//  Neither is in this dump - both are below its depth cut - but both are
//  *referenced* by the NewNode instantiation next door, so something has to
//  define them or the `.mix` does not link (tools/link_check.py's second
//  cause).  They are the same templates Containers/tsarray.cpp and
//  Containers/tslist.cpp already instantiate for the relationship table.
//
//  Their own translation unit, and not NewNode's: Initialize and Rehash
//  call SetCount for real in the shipped stream.
//============================================================================
#include "framereg.h"
#include "tsarray.inl"
#include "tslist.inl"

typedef TSExplicitList<FRAMENAMECREATEHANDLER>       BUCKET;
typedef TSExplicitList<SIMPLEFRAMENAMECREATEHANDLER> SIMPLEBUCKET;

template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
template void TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
template void TSGrowableArray<BUCKET>::SetCount(unsigned int);
template void TSExplicitList<FRAMENAMECREATEHANDLER,
                             TSLIST_LINKOFFSET_POISON>::UnlinkAll();

template unsigned int TSGrowableArray<SIMPLEBUCKET>::ComputeChunk(unsigned int);
template void TSGrowableArray<SIMPLEBUCKET>::SetAlloc(unsigned int);
template void TSGrowableArray<SIMPLEBUCKET>::SetCount(unsigned int);
template void TSExplicitList<SIMPLEFRAMENAMECREATEHANDLER,
                             TSLIST_LINKOFFSET_POISON>::UnlinkAll();
