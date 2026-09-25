//============================================================================
//  The nearest-free-cell placement search (0x6F4A0xxx - 0x6F4A3xxx).
//
//  Given a starting point, a rectangle to stay inside, a collision mask and
//  a footprint size, the search walks outward ring by ring over the pathing
//  grid until it finds a cell the footprint fits in, then hands the cell
//  back as a position.  The three layers are:
//
//    * CPathSearch::TestCell        one cell against the mask, walking the
//                                   grid's registration chain for that cell;
//    * the footprint tests          the 1x1 / 2x2 / 3x3 / 4x4 shapes, each
//                                   with a "moving in direction d" variant
//                                   that only re-tests the cells the move
//                                   brings newly under the footprint;
//    * the ring walkers             four rotations of the same four-sided
//                                   walk around the current rectangle, one
//                                   per starting side.
//
//  Every one of the footprint helpers takes `this` in ecx and never touches
//  it, forwarding it untouched to the next level down - the shipped build's
//  whole-program codegen knew the whole chain preserves ecx.  Spelling them
//  as ordinary members is the closest a single translation unit gets: the
//  argument lists and `retn n` then match exactly and the only cost is the
//  `this` shuffling around each call.  See docs/msvc-vc8-idioms.md,
//  "Arguments in eax and esi".
//============================================================================
#ifndef PATHFIND_H
#define PATHFIND_H

#include "game.h"
#include "spatialgrid.h"
#include "cellbuffer.h"

class SPathPointCache;

//----------------------------------------------------------------------------
//  A cell coordinate pair.  x first, y second - the order every store in the
//  search uses, and the reverse of SCellBox's leading-axis-first field order.
//----------------------------------------------------------------------------
struct SCellPoint
{
    //  Deliberately leaves both members alone: every shipped user of a
    //  local one fills it from a call before reading it, and zeroing it
    //  first costs two stores and a materialised zero the shipped code
    //  does not have (CPathClient::SnapToCell, pathsnaptocell.cpp).
    SCellPoint() {}
    SCellPoint(int x, int y) : m_x(x), m_y(y) {}

    int m_x;
    int m_y;
};

//----------------------------------------------------------------------------
//  The caller's filter.  It is handed the candidate cell already converted
//  to a grid position - two adjacent CFloats, x then y - plus whatever
//  context pointer travelled down with it, and returns non-zero to accept.
//----------------------------------------------------------------------------
typedef int (__fastcall* PathFilterFn)(const CFloat* pos, void* context);

//----------------------------------------------------------------------------
//  The direction bits a footprint test may be given.  A move only brings the
//  cells on the leading edge (or, for a diagonal, the two leading edges plus
//  the corner it cuts) newly under the footprint, so a caller that knows
//  which way the unit came from can name it and skip the rest.  Zero means
//  "no direction known" and tests the whole footprint.
//
//  The four bits are the four axis directions; the four legal diagonals are
//  the four adjacent pairs.  Opposite pairs (5 and 10) and anything with
//  three or more bits set are not moves and fall to the full test.
//----------------------------------------------------------------------------
const int kPathDirSouth = 1;    //  -y
const int kPathDirEast  = 2;    //  +x
const int kPathDirNorth = 4;    //  +y
const int kPathDirWest  = 8;    //  -x

//----------------------------------------------------------------------------
//  0x6F4A0C90/0x6F4A0BA0/0x6F49B6F0 - the coarse search's own frontier: a
//  flat, unordered CMemBlock-backed array of node-record pointers.  Grown
//  and shrunk the same way CCellRecordArray is, just for a plain pointer
//  element instead of SCellRecord - see pathwavefront.cpp.
//----------------------------------------------------------------------------
class SPathFrontier : public CMemBlock
{
public:
    int Grow(const void* fill, unsigned int count);
    void FillFrom(unsigned int start, const void* value, unsigned int end);
    int EraseRange(unsigned int start, unsigned int count);

    void** Nodes() { return (void**)m_data; }

    unsigned int m_growChunk;    // +0x14
    unsigned int m_capacity;     // +0x18
    unsigned int m_count;        // +0x1C
};

//----------------------------------------------------------------------------
//  The search state.  Only the fields this call tree reads are named; the
//  object is far bigger and the rest of it is not recoverable from here.
//----------------------------------------------------------------------------
class CPathSearch
{
public:
    //  --- 0x6F49A470 --------------------------------------------------
    //  Walk the node chain the query produced and push one grid point per
    //  node into the caller's point cache - see pathroutepoints.cpp.
    void BuildRoutePoints(int firstNode, SPathPointCache* cache,
                          const CGridVec2* endPoint,
                          const CGridVec2* startPoint);

    //  --- 0x6F49A800 --------------------------------------------------
    //  The query CPathClient::RunPathQuery drives is CPathAStar::RunSearch
    //  (pathastar.h): this object and that one are the same thing reached
    //  from two directions, so the call site casts rather than declaring a
    //  second entry point to the same address.

    //  --- 0x6F4A0C20 --------------------------------------------------
    //  Is `owner` registered in this cell with a directly-reachable
    //  registration?  See pathcellreaches.cpp.
    int CellReaches(const int* cell, const void* owner);

    //  --- 0x6F4A3990 / 0x6F4A39F0 -------------------------------------
    //  "Does `node` reach this cell", and the expanding-square ring walk
    //  that asks it around a centre - see pathringscan.cpp.  ScanRing walks
    //  the perimeter of the (2*radius+1)-ish square clockwise from its
    //  north-west corner and stops on the first cell that answers yes.
    int CellHasNode(int x, int y, const CGridRegistration* node);
    int ScanRing(int x, int y, const CGridRegistration* node, int radius,
                 int side);

    //  --- 0x6F4A0D60 --------------------------------------------------
    int TestCell(unsigned int x, unsigned int y);

    //  --- 0x6F4A1160 / 0x6F4A11A0 / 0x6F4A11E0 ------------------------
    int TestCellRun(int x, int y, int count);      //  count cells along +x
    int TestCellColumn(int x, int y, int count);   //  count cells along +y
    int TestCellRect(int x, int y, int width, int height);

    //  --- 0x6F4A1230 / 0x6F4A1350 / 0x6F4A14D0 / 0x6F4A1660 -----------
    int TestFootprint1(const SCellPoint* pt, int dir);
    int TestFootprint2(const SCellPoint* pt, int dir);
    int TestFootprint3(const SCellPoint* pt, int dir);
    int TestFootprint4(const SCellPoint* pt, int dir);

    //  --- 0x6F4A1D70 --------------------------------------------------
    //  Latch the mask this search matches cells against, then run the
    //  footprint test for the size class.  `mask` is a pointer because the
    //  shipped code loads it through one - the caller keeps it in a local.
    int TestFootprint(const SCellPoint* pt, const int* mask,
                      unsigned short size);

    //  --- 0x6F4A3060 --------------------------------------------------
    int EvalCandidate(const SCellPoint* pt, const int* mask,
                      unsigned short size, PathFilterFn filter, void* context);

    //  --- 0x6F4A30D0 / 0x6F4A3280 / 0x6F4A3420 / 0x6F4A35C0 -----------
    //  One lap of the rectangle, four starting sides.  Each writes the cell
    //  it is looking at into *pt as it goes and returns 1 as soon as one is
    //  accepted, leaving *pt on it.
    int ScanRingFromTop(const SCellBox* box, SCellPoint* pt, const int* mask,
                        unsigned short size, PathFilterFn filter,
                        void* context);
    int ScanRingFromLeft(const SCellBox* box, SCellPoint* pt, const int* mask,
                         unsigned short size, PathFilterFn filter,
                         void* context);
    int ScanRingFromBottom(const SCellBox* box, SCellPoint* pt,
                           const int* mask, unsigned short size,
                           PathFilterFn filter, void* context);
    int ScanRingFromRight(const SCellBox* box, SCellPoint* pt,
                          const int* mask, unsigned short size,
                          PathFilterFn filter, void* context);

    //  --- 0x6F4A3810 --------------------------------------------------
    //  `pos` is the x/y pair as two adjacent CFloats, the same shape the
    //  filter is handed.
    int TestFootprintAt(const CFloat* pos, const int* mask,
                        unsigned short size);

    //  --- 0x6F4A1EA0 / 0x6F4A1E70 ---------------------------------------
    //  The coarse wavefront's own neighbour test: bounds+mask check a cell
    //  the same way TestCell does, then either record it as blocked or
    //  enqueue the region node behind it - see pathwavefront.cpp.
    void VisitCell(int x, int y, SPathFrontier* frontier);
    void EnqueueNode(SPathFrontier* frontier, void* nodeRecord);

    //  --- 0x6F4A1FA0 / 0x6F4A1FE0 / 0x6F4A2020 ---------------------------
    //  The row/column/rect span iterators VisitCell's four mode dispatchers
    //  (below) share with the placement search's own TestCellRun/Column/
    //  Rect - same shape, an extra frontier argument threaded through.
    void VisitCellRun(int x, int y, SPathFrontier* frontier, int count);
    void VisitCellColumn(int x, int y, SPathFrontier* frontier, int count);
    void VisitCellRect(int x, int y, SPathFrontier* frontier,
                       int width, int height);

    //  --- 0x6F4A2070 / 0x6F4A2180 / 0x6F4A2360 / 0x6F4A2550 --------------
    //  word_6FAB774C's four quality tiers: 1/2/2/3-cell neighbour radius
    //  around the cell the wavefront just expanded from, direction-aware
    //  the same way CPathSearch::TestFootprint1-4 are.
    void VisitNeighbours1(const SCellPoint* pt, int dir, SPathFrontier* f);
    void VisitNeighbours2(const SCellPoint* pt, int dir, SPathFrontier* f);
    void VisitNeighbours3(const SCellPoint* pt, int dir, SPathFrontier* f);
    void VisitNeighbours4(const SCellPoint* pt, int dir, SPathFrontier* f);

    //  --- 0x6F4A3FF0 --------------------------------------------------
    int FindFreePoint(CFloat* out, const SCellBoxF* box, int startSide,
                      const CFloat* radius, const int* mask, int rings,
                      PathFilterFn filter, void* context, int wholeCells);

    //  --- 0x6F4A3E80 --------------------------------------------------
    //  See pathsearchjitter.cpp.
    int FindFreePointNearby(CGridVec2* pos, const SCellBoxF* jitterBox,
                            const CFloat* radius, const int* mask, int rings,
                            PathFilterFn filter, void* context);

    //  --- 0x6F498260 --------------------------------------------------
    //  Read from m_typeRecords[index * 3] (each record three dwords wide,
    //  only the first one read here) and report its low bit. Called from
    //  CPathClient::PopWaypoint (0x6F49C2E0) with a queued-order length as
    //  `index`; what the table's other two dwords per record hold is not
    //  known yet.
    int TestTypeRecordBit(int index) const;

    char                m_reserved00[0x1C];
    CCellGrid*          m_grid;            // +0x1C
    char                m_reserved20[0x3C - 0x20];
    const int*          m_typeRecords;     // +0x3C
    char                m_reserved40[0x5C - 0x40];
    //  +0x5C  the query's own node records, 36 bytes each, indexed by the
    //  chain BuildRoutePoints walks.
    void*               m_routeNodes;      // +0x5C
    char                m_reserved60[0x90 - 0x60];
    //  +0x90  which search mode the query ran in; 2 is the block mode
    //  BuildRoutePoints has to step off block edges for.
    int                 m_reserved90;      // +0x90
    char                m_reserved94[0x9C - 0x94];
    //  +0x9C  how many points the last RunQuery wrote into the caller's
    //  cache; +0xA0 how many of those were the extra "kind" markers
    //  BuildRoutePoints (pathroutepoints.cpp) inserts, which it bumps once
    //  per marker and which CPathClient::RunPathQuery (pathrunquery.cpp)
    //  only ever tests for non-zero.
    unsigned int        m_queryCount;      // +0x9C
    unsigned int        m_extraPoints;     // +0xA0
    //  +0xA4  the mask a cell (and a registration) has to be clear of for
    //  the search to accept it.  Latched by TestFootprint from its caller.
    int                 m_typeMask;        // +0xA4
    //  +0xA8  one registration the search is allowed to overlap - the
    //  object being placed.  Overlapping it is recorded, not rejected.
    CGridRegistration*  m_ignore;          // +0xA8
    //  +0xAC  the frontier above, embedded rather than pointed to - the
    //  0x20-byte CMemBlock-derived object itself, ending exactly where
    //  m_hitIgnore below picks up.
    SPathFrontier       m_frontier;        // +0xAC
    int                 m_hitIgnore;       // +0xCC
    int                 m_blocked;         // +0xD0
    //  +0xD4  set for the duration of one FindFreePoint, and restored by
    //  its __finally.  While set, TestCell stops rejecting registrations
    //  purely for carrying the 0x60000000 flags.
    int                 m_searching;       // +0xD4
};

//----------------------------------------------------------------------------
//  0x6F468700 - the body of FindFreePoint's __finally: put the re-entrancy
//  flag back the way the search found it.  A real function in the shipped
//  build, tail-jumped to from the funclet with the frame pointer in ecx.
//----------------------------------------------------------------------------
struct SPathSearchSave
{
    CPathSearch* m_search;
    int          m_saved;
};

void __fastcall RestoreSearchFlag(SPathSearchSave* save);

//----------------------------------------------------------------------------
//  0x6F4957A0 - the grid-space front end.  Pushes a record describing this
//  search onto the global stack of them, installs its own filter when the
//  caller named a unit to place, and runs FindFreePoint.
//----------------------------------------------------------------------------
int __fastcall FindFreeGridPoint(CFloat* pos, const SCellBoxF* box,
                                 const CFloat* radius, const int* mask,
                                 int placement, int unitType, int player,
                                 int rings, PathFilterFn filter,
                                 void* context, int startSide,
                                 int wholeCells);

//----------------------------------------------------------------------------
//  0x6F473D10 - the world-space front end: bounds-clamp the rectangle, take
//  everything into grid space, search, and take the answer back out.
//----------------------------------------------------------------------------
int __fastcall FindFreeWorldPoint(const SCellBoxF* bounds, CFloat* px,
                                  CFloat* py, const CFloat* radius, int mask,
                                  int startSide, int cellCentres,
                                  PathFilterFn filter, void* context,
                                  int unitType, int player, int placement,
                                  int rings);

//----------------------------------------------------------------------------
//  0x6F473A80 - the jitter-search world-space front end: clamp, take into
//  grid space, retry a growing box centred on the original point up to five
//  times. See pathworldpoint.cpp.
//----------------------------------------------------------------------------
int __fastcall FindFreeWorldPointNearby(CFloat* px, CFloat* py,
                                        const CFloat* radius, int mask,
                                        PathFilterFn filter, void* context);

//----------------------------------------------------------------------------
//  One entry of the global stack of in-flight searches (0x6FAB7548, 0x2C
//  bytes each, depth at 0x6FAB7438).  Only what FindFreeGridPoint writes is
//  named; the filter at 0x6F493CA0 is outside this call tree and is the only
//  thing that reads it back.
//----------------------------------------------------------------------------
struct SPathSearchRecord
{
    CPathSearch* m_owner;        // +0x00
    CFloat       m_cellScale;    // +0x04
    int          m_unitTypeCount;// +0x08
    int          m_unitType;     // +0x0C
    int          m_sizeClass;    // +0x10
    int          m_placement;    // +0x14
    CFloat       m_x;            // +0x18
    CFloat       m_y;            // +0x1C
    PathFilterFn m_filter;       // +0x20
    PathFilterFn m_userFilter;   // +0x24
    void*        m_userContext;  // +0x28
};

extern SPathSearchRecord g_pathSearchStack[];   // unk_6FAB7548
extern int g_pathSearchDepth;                   // dword_6FAB7438

//  0x6F493CA0 - the filter FindFreeGridPoint installs when it was given a
//  unit type to place.  Outside this call tree; only its address is needed.
int __fastcall PathPlacementFilter(const CFloat* pos, void* context);

#endif
