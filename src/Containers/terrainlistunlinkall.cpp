//============================================================================
//  0x6F74F230 / 0x6F74F280 - two more TSExplicitList<T>::UnlinkAll
//  instantiations (the runtime-linkoffset shape - Containers/tslist.inl),
//  reached from the terrain-rendering array releases in this same closure
//  (Misc/citemclosure_terrain_gates.cpp's ReleaseTerrainGridMeshLinkedNodes
//  and ReleaseTerrainImageLinkedNodes each call one of these).
//
//  Neither this dump nor its callers ever put a node into either list or
//  name the owning class, so - exactly as GameUI/herobar.cpp's own comment
//  says for the same shape - the element type here is a placeholder:
//  tslist.inl's generic walk never dereferences T itself, only TSLink<T>'s
//  own two fields, so any type produces the identical instruction stream
//  regardless of what the real owner actually is.  Two distinct incomplete
//  types (not `void`, which would collide with CHeroBar's own
//  TSExplicitList<void, 0> instantiation) - same convention as
//  Widget/widgetlistunlinkall.cpp's SUnlinkedListNode0/1.
//============================================================================
#include "storm.h"
#include "tslist.inl"

struct STerrainListNode0;
struct STerrainListNode1;

//  0x6F74F230
template void TSExplicitList<STerrainListNode0>::UnlinkAll();
//  0x6F74F280
template void TSExplicitList<STerrainListNode1>::UnlinkAll();
