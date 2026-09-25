//============================================================================
//  The per-cell A* search (0x6F4980xx - 0x6F49Axxx).
//
//  This is the *second* of the two search implementations wired into the
//  same PathingClient/mask machinery, and it is not the region-portal
//  wavefront pathmove.h describes.  The wavefront (CPathClient::CoarseSearch,
//  0x6F49D8D0) floods a region graph with no cost ordering at all; this one
//  is a textbook A* over a per-cell node array:
//
//    * a binary min-heap open list (SAStarHeap, 12-byte {key,node,gen}
//      entries, 1-indexed with slot 0 left unused);
//    * real g/h/f scoring stored per node at +0x14/+0x18, both distances
//      taken with the integer square root at 0x6F4BABB0;
//    * lazy deletion instead of decrease-key - every node carries a
//      generation stamp, a popped entry whose stamp no longer matches the
//      node's is dropped and the loop goes round again;
//    * a closed list threaded through the nodes themselves (+0x0C/+0x10 as
//      prev/next, tail at m_closedTail, -3 as the empty sentinel);
//    * parent pointers at +0x1C, so a full route can be walked back, not
//      just a single best point;
//    * and an iteration budget (m_budget/m_iterations) with the same
//      closest-node-to-goal fallback the wavefront keeps.
//
//  Reached from the per-frame steering consumer (0x6F492C90 ->
//  CPathClient::StageAStar (0x6F49DD00) -> 0x6F49B9C0 -> RunSearch), never
//  from the per-tick chain that reaches CoarseSearch.  Both are confirmed
//  to fire live.
//
//  The context object is *not* a global of its own: it is GlobalContext's
//  own member 148 (`*(GlobalContext + 0x250)`, fetched through the plain
//  field getter at 0x6F481460).
//============================================================================
#ifndef PATHASTAR_H
#define PATHASTAR_H

#include "game.h"
#include "cellbuffer.h"
#include "spatialgrid.h"
#include "pathmove.h"

//  Forward-declared rather than pulling in their own full headers here -
//  only pointer parameters need them in this file (same rationale
//  Pathfinding/crlagent.h's own SHandleRegistrationRequest forward
//  declaration gives).
struct SHandleRegistrationRequest;
class CDataStoreScratch;
class CDataStore;

//----------------------------------------------------------------------------
//  One open-list entry.  Passed to SAStarHeap::Push *by value* - the shipped
//  call site builds it in a 12-byte stack temp with `sub esp,0Ch` and three
//  stores through `esp`, which is what MSVC emits for a by-value struct
//  argument and not what it emits for three separate pushes.
//----------------------------------------------------------------------------
//  One hand-placed extra edge.  A node whose m_linkSlot is non-zero indexes
//  this table for a ninth neighbour that is not one of the eight around it.
struct SAStarLink
{
    int m_flags;                // +0x00  bit 0: live
    int m_x;                    // +0x04  the far end
    int m_y;                    // +0x08
};

struct SAStarHeapEntry
{
    unsigned int m_key;         // +0x00  f = g + h
    unsigned int m_node;        // +0x04  index into SAStarNodeArray
    unsigned int m_gen;         // +0x08  the node's generation when pushed
};

//----------------------------------------------------------------------------
//  One search node.  36 bytes - every index computation in this module is
//  the `lea reg,[i+i*8]` / `add reg,reg` / `add reg,reg` triple that is
//  MSVC's canonical *36.
//----------------------------------------------------------------------------
struct SAStarNode
{
    int            m_x;         // +0x00  cell coordinates at m_level
    int            m_y;         // +0x04
    unsigned int   m_gen;       // +0x08  generation stamp (lazy deletion)
    int            m_prev;      // +0x0C  closed-list prev, -1 = not on it
    int            m_next;      // +0x10  closed-list next, -1 = tail
    unsigned int   m_g;         // +0x14  cost from the start
    unsigned int   m_h;         // +0x18  heuristic to the goal
    int            m_parent;    // +0x1C  backtrack chain
    unsigned char  m_linkSlot;  // +0x20  index into CPathAStar::m_links
    unsigned char  m_reserved21;
    unsigned char  m_type;      // +0x22  0 = plain cell, else a portal kind
    unsigned char  m_flag;      // +0x23
};

//----------------------------------------------------------------------------
//  The open list.  The same CMemBlock-backed growable array SPathFrontier
//  (4-byte) and SPathPointCache (8-byte) are, with a 12-byte element.
//----------------------------------------------------------------------------
class SAStarHeap : public CMemBlock
{
public:
    //  0x6F4865B0 - grow by `count` entries, filling the new ones from
    //  `*fill` when it is not null.
    int Grow(const SAStarHeapEntry* fill, unsigned int count);

    //  0x6F498060 - shift-erase [start, start+count).
    int EraseRange(unsigned int start, unsigned int count);

    //  0x6F497CC0 - hand the spare capacity back, returning how many bytes
    //  that freed.
    int ShrinkToFit();

    //  0x6F483FF0 - fill [start, min(start+count, m_count)) from one
    //  repeated entry.  Three dword moves per slot, not a rep movsd - at 12
    //  bytes MSVC still unrolls the struct assignment.
    void FillFrom(unsigned int start, const SAStarHeapEntry* value,
                  unsigned int count);

    //  0x6F498580 - push one entry, percolating it up.
    void Push(SAStarHeapEntry entry);

    //  0x6F4985F0 - pop the minimum into `*out`, percolating the old last
    //  entry back down.  Returns `out`.
    SAStarHeapEntry* PopMin(SAStarHeapEntry* out);

    SAStarHeapEntry* Entries() { return (SAStarHeapEntry*)m_data; }

    unsigned int m_growChunk;    // +0x14
    unsigned int m_capacity;     // +0x18
    unsigned int m_count;        // +0x1C
};

//----------------------------------------------------------------------------
//  The extra-edge table's own growable storage - the SAME CMemBlock-backed
//  array shape as SAStarHeap above, instantiated again for the 12-byte
//  SAStarLink element, at its own three addresses
//  (0x6F498100/0x6F498180/0x6F497780) rather than folded into SAStarHeap's -
//  this image was linked without /OPT:ICF (pathastarheaparray.cpp's own
//  header note makes the same point about SAStarHeap's own four methods), so
//  a second instantiation of the identical template over a same-size element
//  gets its own three real functions rather than sharing SAStarHeap's.
//
//  IMPORTANT - this is CPathAStar's own `m_links` field's real underlying
//  storage, not a second, unrelated member: `SAStarLinkArray`'s inherited
//  CMemBlock::m_data sits at local +0x0C, i.e. CPathAStar's own +0x3C -
//  exactly where `m_links` (declared below as a bare `SAStarLink*`) already
//  lives and is already read by every already-scoring caller in this file
//  (pathastarexpand.cpp's `m_links[linkSlot]`). CPathAStar::~CPathAStar
//  (pathastar_dtor.cpp, this session) tears the whole 0x20-byte object down
//  at +0x30 through the polymorphic CMemBlock base destructor, and
//  Apply/CancelAndUnregister/Save/Load (this session, batch E follow-up 5)
//  all grow/erase/serialize it as a real CMemBlock-derived array - which is
//  what pins its real span to +0x30..+0x50, engulfing `m_links`'s own
//  storage rather than sitting next to it. Left the class's own member list
//  unchanged (a `SAStarLink* m_links` pointer, not an embedded
//  `SAStarLinkArray m_links`) rather than risk a working, already-scoring
//  caller's codegen on a layout change with no dump-level evidence either
//  way for the pointer-vs-embedded spelling; every new function in this
//  batch reaches the object through `(SAStarLinkArray*)((char*)this+0x30)`
//  instead. A later session that wants a single, formally correct member can
//  fold the two once it has verified that reshaping does not move
//  pathastarexpand.cpp off its current score.
//----------------------------------------------------------------------------
class SAStarLinkArray : public CMemBlock
{
public:
    //  0x6F498100 - same shape as SAStarHeap::Grow (pathastarheaparray.cpp).
    int Grow(const SAStarLink* fill, unsigned int count);

    //  0x6F498180 - same shape as SAStarHeap::EraseRange.
    int EraseRange(unsigned int start, unsigned int count);

    //  0x6F497780 - same shape as SAStarHeap::FillFrom.
    void FillFrom(unsigned int start, const SAStarLink* value,
                  unsigned int count);

    SAStarLink* Entries() { return (SAStarLink*)m_data; }

    unsigned int m_growChunk;    // +0x14
    unsigned int m_capacity;     // +0x18
    unsigned int m_count;        // +0x1C
};

//----------------------------------------------------------------------------
//  The node pool, same array again with a 36-byte element.
//----------------------------------------------------------------------------
class SAStarNodeArray : public CMemBlock
{
public:
    //  0x6F4984E0 - grow by `count` nodes.
    int Grow(const SAStarNode* fill, unsigned int count);

    //  0x6F498710 - shift-erase [start, start+count).
    int EraseRange(unsigned int start, unsigned int count);

    //  0x6F497C90 - hand the spare capacity back.
    int ShrinkToFit();

    //  0x6F498010 - fill [start, min(start+count, m_count)) from one
    //  repeated node.  At 36 bytes the struct assignment turns into a
    //  `mov ecx,9` / `rep movsd` rather than nine explicit moves.
    void FillFrom(unsigned int start, const SAStarNode* value,
                  unsigned int count);

    SAStarNode* Nodes() { return (SAStarNode*)m_data; }

    unsigned int m_growChunk;    // +0x14
    unsigned int m_capacity;     // +0x18
    unsigned int m_count;        // +0x1C
};

//----------------------------------------------------------------------------
//  One cell of the four-level grid pyramid this search runs over.  Eight
//  bytes: a stamp saying which search last touched it, and a dword whose top
//  bits (once shifted by the context's own m_maskShift) are the passability
//  class and whose bottom word is scratch - the index of the node registered
//  for this cell by the current search.
//----------------------------------------------------------------------------
struct SAStarCell
{
    int m_stamp;                // +0x00  == CPathAStar::m_searchStamp
    union
    {
        unsigned int   m_word;  // +0x04  read whole for the class test
        unsigned short m_node;  // +0x04  written alone as the node index
    };

    //  +0x06 - the low byte of the class half, which is also the grid level
    //  a node registered for this cell inherits.
    unsigned char LevelByte() const
    {
        return ((const unsigned char*)&m_word)[2];
    }
};

//  The pyramid level itself.  Not CCellGrid (spatialgrid.h) despite sharing
//  three offsets with it - that one's +0x28 is an array of 4-byte cell words,
//  this one's is an array of 8-byte SAStarCell.
struct SAStarGrid
{
    char       m_reserved00[0x28];
    SAStarCell* m_cells;        // +0x28
    char       m_reserved2C[0x3C - 0x2C];
    int        m_stride;        // +0x3C  cells per row
    int        m_rows;          // +0x40

    //  0x6F497D30 - the centre of `cell`, as two grid-space CFloats.  A
    //  member whose `this` is never read - the shipped callers load ecx with
    //  this grid all the same, so it is a method and not a free function
    //  (the same dead-`this` shape CCellGrid::FloatBoxToCellBox has).
    CFloat* CellCentre(CFloat* out, const SCellPoint* cell);

    //  Bounds-checked cell fetch, null when the cell is off the grid.
    //  Inlined at all three of its call sites in the shipped build.
    SAStarCell* CellAt(int x, int y) const
    {
        if ((unsigned int)x >= (unsigned int)m_stride)
            return 0;
        if ((unsigned int)y >= (unsigned int)m_rows)
            return 0;
        return m_cells + (m_stride * y + x);
    }
};

class CPathAStar;

//----------------------------------------------------------------------------
//  The search context itself.
//----------------------------------------------------------------------------
class CPathAStar
{
public:
    //  ---- NIpse::CLrPathingAcc vtable slots (batch E, this session) --
    //  RTTI-confirmed identity: `agent_worktrees/classes/0x6F95273C.json`
    //  (NIpse::CLrPathingAcc, 17 slots) gives slot 3's own target as
    //  0x6F49A330, which zeroes exactly `m_grids[4]` (+0x1C) and
    //  `m_searchStamp` (+0x2C) after registering - and slot 4's own target
    //  (0x6F49A600) calls straight into this class's own already-
    //  reconstructed `Reset(1)` with `this` unchanged (real thiscall, ecx
    //  not adjusted) and zeroes the same four grid pointers again - both
    //  only make sense if CPathAStar, provisionally named long before this
    //  vtable was visible, IS NIpse::CLrPathingAcc. Left named CPathAStar
    //  for the same reason CPathClient/CLrPath is (pathmove.h's own class
    //  comment).
    //
    //  Slot 0 (GetTypeName, 0x6F488B50) is a free function below.

    //  Slot 1 (0x6F488BA0) - same unlink-then-return-to-pool shape as the
    //  rest of this cluster.
    void ReturnToStaticPool();

    //  0x6F488BE0 - the real destructor, reached through slot 2's scalar
    //  deleting destructor below.  See pathastar_dtor.cpp for the SEH-frame
    //  caveat.
    ~CPathAStar();

    //  Slot 2 (0x6F489300) - scalar deleting destructor.  Same shape as
    //  NIpse::CRlAgent::ScalarDeletingDestructor (crlagent_scalardtor.cpp).
    void* ScalarDeletingDestructor(unsigned int flags);

    //  Slot 3 (0x6F49A330) - register.  Delegates to the handle-table base
    //  the same shape CRlAgent::Apply/CPathClient::Apply already establish,
    //  then zeroes the four `m_grids` pointers and `m_searchStamp`, then
    //  grows the +0x30 link array (SAStarLinkArray, above) to a flat 256
    //  zero-filled slots.
    void Apply(SHandleRegistrationRequest* request);

    //  Slot 4 (0x6F49A600) - cancel.  Calls straight into this class's own
    //  already-reconstructed `Reset(1)` with `this` unchanged (real
    //  thiscall, no `ecx` adjustment - the observation that pins CPathAStar's
    //  own identity to NIpse::CLrPathingAcc), zeroes the same four grid
    //  pointers again, empties the +0x30 link array back to zero elements,
    //  then unregisters through the handle-table base.
    void CancelAndUnregister(int arg);

    //  Slot 9 (0x6F498460) - Save.  Slot 10 (0x6F4984A0) - Load.  Both in
    //  pathastar_save.cpp/pathastar_load.cpp.
    void Save(CDataStoreScratch* store);
    void Load(CDataStore* store);

    //  --- 0x6F498A60 / 0x6F498A00 -------------------------------------
    //  Resolve a cell at `level` to a node index, walking up to coarser
    //  levels when the cell it lands in is not itself passable.  Returns
    //  -1 when the cell is out of bounds or blocked outright.
    int LookupNode(unsigned int level, int x, int y);
    void RegisterNode(const void* cell, unsigned int level, int x, int y);

    //  --- 0x6F498C70 / 0x6F498C20 -------------------------------------
    //  Offer `node` a route through `parent`, taking it if that is an
    //  improvement (pathastarrelax.cpp).  Unlink splices a node out of the
    //  closed list it is currently on and bumps its generation.
    void Relax(int node, int parent);
    void Unlink(int node);

    //  --- 0x6F498B80 / 0x6F498BD0 -------------------------------------
    //  Take `node` off the open list (bumping its generation so any heap
    //  entry still naming it goes stale) and push its {f,idx,gen} on.
    void Reopen(int node);
    //  Append `node` to the closed list.
    void Close(int node);

    //  --- 0x6F499E90 / 0x6F499FA0 -------------------------------------
    //  The two expanders RunLoop dispatches between on a node's m_type: the
    //  plain 8-connected cell walk (pathastarexpand.cpp) and the portal form.
    void ExpandCell(int node, int x, int y, int linkSlot);
    void ExpandPortal(int node, int level, int x, int y);

    //  0x6F4998F0 / 0x6F4999F0 / 0x6F499AF0 / 0x6F499BF0 - the portal
    //  expander's four edge walks (pathportalspans.cpp), each reporting
    //  whether its low and high ends came out reachable; 0x6F499CF0 /
    //  0x6F499D60 / 0x6F499DD0 / 0x6F499E40 - the four corners those flags
    //  gate (pathportalcorners.cpp).
    void SpanNorth(int node, int level, int x, int y, int* outLo, int* outHi);
    void SpanEast(int node, int level, int x, int y, int* outLo, int* outHi);
    void SpanSouth(int node, int level, int x, int y, int* outLo, int* outHi);
    void SpanWest(int node, int level, int x, int y, int* outLo, int* outHi);
    void PortalNorthEast(int node, int level, int x, int y);
    void PortalSouthEast(int node, int level, int x, int y);
    void PortalSouthWest(int node, int level, int x, int y);
    void PortalNorthWest(int node, int level, int x, int y);

    //  0x6F4991C0 / 0x6F4992F0 / 0x6F499210 / 0x6F499380 / 0x6F499410 /
    //  0x6F4992A0 / 0x6F4994A0 / 0x6F499530 / 0x6F499590 - the portal
    //  expander's own nine gates (pathportalgates.cpp, and
    //  pathportalgateeast.cpp for the one two of the others call).
    int PortalGateNorth(int x, int y);
    int PortalGateNorthEdge(int x, int y);
    int PortalGateEast(int x, int y);
    int PortalGateEastEdge(int x, int y);
    int PortalGateSouthEdge(int x, int y);
    int PortalGateWest(int x, int y);
    int PortalGateWestEdge(int x, int y);
    int PortalGateNorthEastCorner(int x, int y);
    int PortalGateSouthWestCorner(int x, int y);

    //  0x6F4995E0 / 0x6F499660 / 0x6F4996C0 / 0x6F499720 - the four cardinal
    //  neighbour tests (pathastarneighbours.cpp), non-zero when the step is
    //  possible; 0x6F4997A0 / 0x6F4997F0 / 0x6F499840 / 0x6F499890 - the four
    //  diagonals, run only when both flanking cardinals passed; 0x6F4998C0 -
    //  the link-table neighbour.  All nine relax the node they reach; only
    //  the cardinals' results are ever read.
    int TestNorth(int parent, int x, int y);
    int TestEast(int parent, int x, int y);
    int TestSouth(int parent, int x, int y);
    int TestWest(int parent, int x, int y);
    void TestNorthEast(int parent, int x, int y);
    void TestSouthEast(int parent, int x, int y);
    void TestSouthWest(int parent, int x, int y);
    void TestNorthWest(int parent, int x, int y);
    void TestLink(int parent, int x, int y);

    //  0x6F498890 - "is this cell passable for the unit this search is
    //  for".  Every gate in pathastargates.cpp carries its own open-coded
    //  copy of this test (see ASTAR_CELL_OPEN below) and calls this one only
    //  for a *second* cell.
    int IsCellOpen(int x, int y);

    //  0x6F498E80 / 0x6F498ED0 / 0x6F498F30 / 0x6F498F90 - one passability
    //  gate per cardinal, and 0x6F498FE0 / 0x6F499050 the second gate north
    //  and west take on a half-cell step; 0x6F4990C0 / 0x6F499110 /
    //  0x6F499170 the three diagonals' own.  All nine only run at mode 2.
    int GateNorth(int x, int y);
    int GateEast(int x, int y);
    int GateSouth(int x, int y);
    int GateWest(int x, int y);
    int GateNorthHalf(int x, int y);
    int GateWestHalf(int x, int y);
    int GateNorthEast(int x, int y);
    int GateSouthEast(int x, int y);
    int GateSouthWest(int x, int y);

    //  0x6F498D90 - the link neighbour's own relax.
    void RelaxLink(int node, int parent);

    //  --- 0x6F49A800 / 0x6F49A640 --------------------------------------
    //  The whole search, from the caller's point of view: empty the output
    //  cache, set the endpoints up, and either answer straight away or run
    //  the loop and walk the result back into a route.
    int RunSearch(int maskShift, SPathPointCache* out, const CGridVec2* from,
                  const CGridVec2* to, int budget, int tier,
                  int useCellLevel);

    //  Reset, latch the endpoints and the mask shift, quantise both to
    //  cells, and classify: 0 unusable, 1 trivial (nothing to search),
    //  2 a real search is needed.
    int Setup(int maskShift, const CGridVec2* from, const CGridVec2* to,
              int budget, int tier, int useCellLevel);

    //  0x6F49A470 is *not* declared here.  It already has a name and a
    //  reconstruction as CPathSearch::BuildRoutePoints (pathfind.h,
    //  pathroutepoints.cpp), and CPathSearch and CPathAStar are the same
    //  object reached from two directions - so RunSearch casts at the call
    //  site rather than giving one shipped function two C++ entry points.
    //  A second declaration compiles and scores fine and then fails to
    //  link, which is the whole class of defect
    //  docs/notes/verifier-gate-link-failures.md exists for.

    //  0x6F49A5E0 - append one point to `cache`.  `this` is never read; see
    //  pathqueuepoint.cpp for why it is a member all the same.
    void QueuePoint(SPathPointCache* cache, const CGridVec2* point);

    //  --- 0x6F49A240 --------------------------------------------------
    //  Give the destination node its heuristic, push it, and run the loop.
    int Begin();

    //  --- 0x6F49A3F0 --------------------------------------------------
    //  Empty both arrays back to a fresh search, optionally handing the
    //  spare capacity back to the allocator.
    void Reset(int shrink);

    //  --- 0x6F49A170 --------------------------------------------------
    //  The main loop: pop, skip stale, expand, close, until the goal comes
    //  off the heap or the budget runs out.
    int RunLoop();

    SAStarNode* Nodes() { return m_nodes.Nodes(); }

    char             m_reserved00[0x1C];
    SAStarGrid*      m_grids[4];        // +0x1C  coarsest last
    int              m_searchStamp;     // +0x2C  bumped once per search
    char             m_reserved30[0x3C - 0x30];
    SAStarLink*      m_links;           // +0x3C
    char             m_reserved40[0x50 - 0x40];
    SAStarNodeArray  m_nodes;           // +0x50
    SAStarHeap       m_heap;            // +0x70
    //  1 << the quality tier the caller asked for, so the gates' own
    //  `m_mode == 2` test is "tier 1".
    int              m_mode;            // +0x90
    int              m_closedTail;      // +0x94  -3 when the list is empty
    unsigned int     m_budget;          // +0x98  iterations allowed
    unsigned int     m_iterations;      // +0x9C  iterations spent
    unsigned int     m_expansions;      // +0xA0  nodes expanded this search
    //  The two endpoints as the caller gave them, before quantising.  m_to
    //  is overwritten with the best reachable node's cell centre when the
    //  goal itself could not be reached - see CPathAStar::RunSearch.
    CGridVec2        m_from;            // +0xA4
    CGridVec2        m_to;              // +0xAC
    int              m_startX;          // +0xB4  the search's own start cell
    int              m_startY;          // +0xB8
    int              m_destX;           // +0xBC  and its destination cell
    int              m_destY;           // +0xC0
    int              m_startNode;       // +0xC4
    int              m_destNode;        // +0xC8  the goal's node index
    unsigned int     m_bestDistSq;      // +0xCC  closest-to-goal fallback
    int              m_bestNode;        // +0xD0
    unsigned int     m_maskShift;       // +0xD4
    int              m_useCellLevel;    // +0xD8  take the level off the cell
    char             m_reservedDC[0xE0 - 0xDC];
};

//----------------------------------------------------------------------------
//  The cell-passability test, as a macro rather than a function.
//
//  The shipped build has this open-coded inside every one of the nine gates
//  in pathastargates.cpp *and* standing on its own at 0x6F498890, which the
//  gates call when they need a second cell.  A shared inline member does not
//  reproduce that: MSVC at /O2 declines to inline a body this size, emits one
//  out-of-line copy and tail-jumps to it from every gate, and all nine stop
//  matching.  A macro is how the duplication gets said once instead of nine
//  times; the alternative is nine hand-copied bodies, or a __forceinline the
//  shipping build had no way to spell.
//
//  Passability is the top two bits of the cell's flag dword after the
//  per-search shift that stages the unit's PathingType mask into a fixed
//  position: zero means open, anything else blocked.
//
//  The cell address appears twice; the compiler folds the two copies.
//----------------------------------------------------------------------------
#define ASTAR_CELL_OPEN(g, cx, cy)                                            \
    ((unsigned int)(cx) < (unsigned int)(g)->m_stride &&                      \
     (unsigned int)(cy) < (unsigned int)(g)->m_rows &&                        \
     (g)->m_cells + ((g)->m_stride * (cy) + (cx)) != 0 &&                     \
     (((((g)->m_cells + ((g)->m_stride * (cy) + (cx)))->m_word)               \
        << m_maskShift) & 0xC0000000) == 0)

//  unk_6FAB7744 - the discard sink the span testers redirect their "low end"
//  out-parameter to once the low sub-span has had its turn.  Nothing ever
//  reads it; see pathportalspans.cpp.
extern int g_astarSpanSink;

//  0x6F4BABB0 - integer square root by Newton's method (pathisqrt.cpp).
unsigned int __fastcall IntegerSqrtNewton(unsigned int value);

//  0x6F488B50 - NIpse::CLrPathingAcc's own vtable slot 0.  `this` never
//  read; returns the literal string "LrPathingAcc".
const char* PathAStar_GetTypeName();

#endif
