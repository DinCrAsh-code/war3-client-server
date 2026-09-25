//============================================================================
//  0x6F49A5E0 / 0x6F4A3040 - append one point to a search's point cache.
//
//  Two byte-identical copies in two different modules, each a single
//  SPathPointCache::Grow of one slot filled from the caller's point.
//
//  Both take the cache as an ordinary first argument and neither reads
//  `this` - but every shipped call site loads ecx with the search context
//  first, so both are methods with a dead `this` rather than free functions.
//  Declaring them free costs each caller the `mov ecx,esi` that sets it.
//============================================================================
#include "pathfinesearch.h"

//  0x6F49A5E0 - the per-cell A*'s copy.
void CPathAStar::QueuePoint(SPathPointCache* cache, const CGridVec2* point)
{
    cache->Grow(point, 1);
}

//  0x6F4A3040 - the fine search's copy.
void CPathFineSearch::QueuePoint(SPathPointCache* cache,
                                 const CGridVec2* point)
{
    cache->Grow(point, 1);
}
