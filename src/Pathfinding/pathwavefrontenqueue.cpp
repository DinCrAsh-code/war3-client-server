//============================================================================
//  0x6F4A1E70 - CPathSearch::EnqueueNode: add a node record to the frontier
//  once, capped and deduplicated.
//
//  The shipped build passes `nodeRecord` in `eax` rather than through any
//  callable convention - VisitCell (pathwavefrontvisit.cpp) leaves it there
//  from the load that produced it and this callee just reads it, the
//  whole-program-codegen idiom docs/msvc-vc8-idioms.md calls "Arguments in
//  eax and esi". No declarable calling convention reaches `eax` for a
//  second argument, so this reconstruction takes it normally and the
//  `retn` size differs by that one argument's four bytes - verdict
//  IDENTICAL, not EXACT.
//
//  The dedup check is a single fixed tag, not a search: a node record's own
//  first field (+0x10, the same "type tag" shape CHandleObject::m_typeTag
//  uses elsewhere in this repo) is compared against a fixed FourCC-looking
//  constant, and only a record already carrying it is trusted as "already
//  queued". A null node record, or one that has not been tagged yet, is
//  queued unconditionally - the tag only ever prevents a second enqueue of
//  the *same* record already sitting in the frontier from an earlier visit
//  this search.
//============================================================================
#include "pathfind.h"

const int kPathNodeTag = 0x60706375;

void CPathSearch::EnqueueNode(SPathFrontier* frontier, void* nodeRecord)
{
    if (frontier->m_count >= 0x20)
        return;

    void* queue = 0;
    if (nodeRecord != 0 &&
        *(int*)((char*)nodeRecord + 0x10) == kPathNodeTag)
        queue = nodeRecord;

    frontier->Grow(&queue, 1);
}
