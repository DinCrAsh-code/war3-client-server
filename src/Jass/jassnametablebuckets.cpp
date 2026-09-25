//============================================================================
//  The name table's bucket array - TSGrowableArray of TSExplicitList, so
//  every element has to be constructed, destroyed and relinked by hand
//  rather than memcpy'd.  0x6F449900 ComputeChunk, 0x6F44E500 SetAlloc,
//  0x6F44F5D0 SetCount - the same three-member split
//  Render/prefetcharray.cpp uses for the file cache's own bucket array.
//============================================================================
#include "jasssymtables.h"
#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@UJASSNAMENODE@@$0?CCCCCCCD@@@"
#include "tsarray.inl"
#include "tslist.inl"

typedef TSExplicitList<JASSNAMENODE> BUCKET;

template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
template void         TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
template void         TSGrowableArray<BUCKET>::SetCount(unsigned int);

//  UnlinkAll() is what SetLinkOffset() calls on a bucket the first time its
//  own link offset is set (storm.h) - a real out-of-line call at every one
//  of Initialize's four bucket-setup sites, so this table's own link needs
//  it defined regardless of which address it lands at in the shipped image.
template void TSExplicitList<JASSNAMENODE>::UnlinkAll();
