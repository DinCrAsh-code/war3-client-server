//============================================================================
//  The handle-to-agent table's out-of-line members: everything in the
//  0x6F42A000-0x6F42FFFF neighbourhood that sub_6F430C80's call tree reaches
//  through it, plus its NewNode (0x6F430580, next to the root itself).
//============================================================================
#include "handle2agent.h"
#include "tshash.inl"
#include "tsarray.inl"
#include "tslist.inl"

typedef TSExplicitList<HANDLE2AGENT> BUCKET;

// 0x6F42EFF0
template void TSHashTable<HANDLE2AGENT, HANDLE_ID>::Initialize();
// 0x6F42FD30
template int  TSHashTable<HANDLE2AGENT, HANDLE_ID>::CheckGrow(unsigned int);
// 0x6F42F170
template void TSHashTable<HANDLE2AGENT, HANDLE_ID>::Rehash(unsigned int);
// 0x6F430580
template HANDLE2AGENT* TSHashTable<HANDLE2AGENT, HANDLE_ID>::NewNode(
    unsigned int, void*, void*);
// 0x6F42CCF0
template HANDLE2AGENT* TSHashTable<HANDLE2AGENT, HANDLE_ID>::PtrHashOnly(
    unsigned int, const HANDLE_ID&);

// 0x6F42A0C0
template void TSExplicitList<HANDLE2AGENT>::UnlinkAll();

// 0x6F4278B0
template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
// 0x6F42BBD0
template void TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
// 0x6F42D690
template void TSGrowableArray<BUCKET>::SetCount(unsigned int);
