//============================================================================
//  TSExplicitList::UnlinkAll for the relationship list (0x6F46B560).
//============================================================================
#include "agiletype.h"
#include "tslist.inl"

typedef AGILE_TYPE_DATA::AGILE_TYPE_RELATIONSHIP REL;

template void TSExplicitList<REL, TSLIST_LINKOFFSET_POISON>::UnlinkAll();
