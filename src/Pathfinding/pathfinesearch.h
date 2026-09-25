//============================================================================
//  The fine per-cell A* (0x6F4A18xx - 0x6F4A2Exx).
//
//  A second, complete instance of the search pathastar.h describes: the same
//  binary min-heap, the same 36-byte SAStarNode with its generation stamp,
//  parent pointer and g/h scores, the same lazy-deletion pop loop, the same
//  closed list threaded through the nodes with -3 as its empty sentinel.
//  Only the context object differs - this one carries the arrays at +0x24 and
//  +0x44 where CPathAStar has them at +0x50 and +0x70 - which is why every
//  array routine in pathastar.h is shared between the two and only the
//  context-level methods are duplicated here.
//
//  What this instance adds is the neighbour generation the other one reaches
//  through a different pair of expanders: four size-indexed 8-neighbour
//  walks, picked by m_footprint, each of which asks one perimeter query for
//  the whole ring around the unit's footprint and then reads the eight
//  directions back out of that one bitmask.
//
//  The perimeter is numbered clockwise from the north-west corner: corner,
//  then `size` cells of north edge, then corner, `size` of east, and so on,
//  so a footprint of `size` has 4 * (size + 1) bits.  A cardinal step needs
//  its own edge clear; a diagonal step needs its corner *and* both flanking
//  edges clear, which is the standard no-cutting-through-a-corner rule and
//  is where all four functions' otherwise arbitrary-looking masks come from.
//============================================================================
#ifndef PATHFINESEARCH_H
#define PATHFINESEARCH_H

#include "game.h"
#include "pathfind.h"
#include "pathastar.h"
#include "spatialgrid.h"
#include "pathmove.h"

class CPathFineSearch
{
public:
    //  ---- NIpse::CLrPathingSys vtable slots (batch E, this session) --
    //  RTTI-confirmed identity: `agent_worktrees/classes/0x6F95359C.json`
    //  (NIpse::CLrPathingSys, 17 slots) gives slot 3's own target as
    //  0x6F4A0710, which zeroes exactly `m_grid` (+0x1C) and `m_searchTag`
    //  (+0x20) after registering - and slot 4's own target
    //  (0x6F4A3960) calls straight into this class's own already-
    //  reconstructed `Reset(1)` with `this` unchanged (real thiscall, ecx
    //  not adjusted) - both only make sense if CPathFineSearch, provisionally
    //  named long before this vtable was visible (CLAUDE.md's own "Caveat
    //  for classes/"), IS NIpse::CLrPathingSys. Left named CPathFineSearch
    //  for the same reason CPathClient/CLrPath is left named CPathClient -
    //  see pathmove.h's own class comment.
    //
    //  Slot 2 (ScalarDeletingDestructor) is inherited unchanged straight
    //  from NTempest::CPresence (agent_worktrees data: CPresence's own
    //  target address, 0x6F4A3760, appears verbatim in this class's own
    //  vtable at that slot) - nothing to add here.

    //  Slot 0 (GetTypeName, 0x6F4A1C60) is a free function below.

    //  Slot 1 (0x6F4A1CB0) - same unlink-then-return-to-pool shape as the
    //  rest of this cluster (Pathfinding/pathclient_returntopool.cpp).
    void ReturnToStaticPool();

    //  Slot 3 (0x6F4A0710) - register into the handle table
    //  (SHandleTableEntry::RegisterOrReuse, handletable.h), then reset the
    //  two fields this class's own vtable methods touch.
    void Apply(struct SHandleRegistrationRequest* request);

    //  Slot 4 (0x6F4A3960) - tear the search down (Reset(1), already
    //  reconstructed above), drop the grid pointer, unregister and notify.
    void CancelAndUnregister(int arg);

    //  Slot 9 (0x6F4A0A50) / slot 10 (0x6F4A0A80) - serialize m_grid
    //  through the same pathing-reference-entry helper the base chain's
    //  own registrations use (Pathfinding/handletable.h's own
    //  "0x6F4AEF40 - write one pathing-reference entry" doc), then
    //  m_searchTag as a plain word.
    void Save(class CDataStoreScratch* store);
    void Load(class CDataStore* store);

    //  --- 0x6F4A1080 / 0x6F4A1800 -------------------------------------
    //  One query for the whole ring around the footprint at (x, y),
    //  returning a bit per perimeter cell: set means blocked.  `inner` and
    //  `outer` bound the ring; the four callers pass (1,3), (2,4), (2,5)
    //  and (3,6) - outer is size + 2 and inner is outer / 2.
    int TestPerimeter(int x, int y, int inner, int outer);

    //  0x6F4A1800 - resolve a cell to a node index, registering one if this
    //  search has not been to the cell yet.  -1 when the cell is off the
    //  grid or the node pool is already at its 0x8000 ceiling.
    int LookupNode(int x, int y);

    //  --- 0x6F4A2750 / 0x6F4A2880 / 0x6F4A29C0 / 0x6F4A2B00 -----------
    //  The eight neighbours of `at` for each footprint size, in the fixed
    //  order NW, N, NE, W, E, SW, S, SE.  A direction the perimeter mask
    //  rules out is written back as -1 rather than skipped, so the caller
    //  can index the array by direction.
    void Neighbours1(const SCellPoint* at, int* out);
    void Neighbours2(const SCellPoint* at, int* out);
    void Neighbours3(const SCellPoint* at, int* out);
    void Neighbours4(const SCellPoint* at, int* out);

    //  --- 0x6F4A2C50 / 0x6F4A19B0 -------------------------------------
    //  Generate this node's neighbours at the current footprint size and
    //  relax each one that came back.
    void Expand(int node);
    void Relax(int node, int parent, int cost);

    //  --- 0x6F4A18D0 / 0x6F4A1920 / 0x6F4A1960 ------------------------
    void Reopen(int node);
    void Close(int node);
    void Unlink(int node);

    //  --- 0x6F4A2DA0 --------------------------------------------------
    //  Empty both arrays back to a fresh search, optionally handing the
    //  spare capacity back to the allocator.
    void Reset(int shrink);

    //  0x6F4A3040 - append one point to `cache`; see pathqueuepoint.cpp.
    void QueuePoint(SPathPointCache* cache, const CGridVec2* point);

    //  --- 0x6F4A2F30 --------------------------------------------------
    //  Walk the parent chain back from `node`, appending each cell's centre
    //  to `out`, then pin the two ends to `from` and `to`.
    void BuildRoute(int node, SPathPointCache* out, const CGridVec2* from,
                    const CGridVec2* to);

    //  --- 0x6F4A3CC0 / 0x6F4A3AD0 -------------------------------------
    //  The whole search as its callers see it, and the setup step under it.
    //  See CPathAStar::RunSearch (pathastarrun.cpp) for the four outcomes;
    //  this one differs only in which of its own arguments go where.
    int RunSearch(SPathPointCache* out, const CGridVec2* from,
                  const CGridVec2* to, const int* mask, int budget,
                  const CFloat* radius, int typeFlags);
    int Setup(const CGridVec2* from, const CGridVec2* to, int budget,
              const int* mask, const CFloat* radius, int typeFlags);

    //  --- 0x6F4A2EA0 --------------------------------------------------
    //  Give the start node its heuristic, push it, and run the loop.
    int Begin();

    //  --- 0x6F4A2E00 --------------------------------------------------
    int RunLoop();

    SAStarNode* Nodes() { return m_nodes.Nodes(); }

    char             m_reserved00[0x1C];
    CCellGrid*       m_grid;            // +0x1C
    unsigned short   m_searchTag;       // +0x20  bumped once per search
    char             m_reserved22[0x24 - 0x22];
    SAStarNodeArray  m_nodes;           // +0x24
    SAStarHeap       m_heap;            // +0x44
    int              m_closedTail;      // +0x64  -3 when the list is empty
    unsigned int     m_budget;          // +0x68
    unsigned int     m_iterations;      // +0x6C
    //  The two endpoints as the caller gave them, before quantising.  m_to
    //  is overwritten with the best reachable node's cell centre when the
    //  goal itself could not be reached - see CPathFineSearch::RunSearch.
    CGridVec2        m_from;            // +0x70
    CGridVec2        m_to;              // +0x78
    int              m_startX;          // +0x80  the search's own start cell
    int              m_startY;          // +0x84
    int              m_destX;           // +0x88  the goal cell
    int              m_destY;           // +0x8C
    int              m_startNode;       // +0x90
    int              m_destNode;        // +0x94
    unsigned int     m_bestDistSq;      // +0x98  closest-to-goal fallback
    int              m_bestNode;        // +0x9C
    unsigned short   m_footprint;       // +0xA0  0-3, picks Neighbours1-4
    char             m_reservedA2[0xA4 - 0xA2];
    int              m_typeMask;        // +0xA4  the unit's PathingType mask
    int              m_typeFlags;       // +0xA8
    char             m_reservedAC[0xCC - 0xAC];
    int              m_abort;           // +0xCC  set by a relax that failed
    int              m_reservedD0;      // +0xD0  cleared with m_abort
    int              m_reservedD4;      // +0xD4
};

//  0x6F4A1C60 - NIpse::CLrPathingSys's own vtable slot 0.  `this` never
//  read; returns the literal string "LrPathingSys".
const char* PathFineSearch_GetTypeName();

#endif
