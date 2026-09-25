//============================================================================
//  TSExplicitList::UnlinkAll for the footprint type table's buckets
//  (0x6F313690).
//============================================================================
#include "footprinttype.h"
#include "tslist.inl"

template void TSExplicitList<CFootprintTypeData, TSLIST_LINKOFFSET_POISON>::UnlinkAll();
