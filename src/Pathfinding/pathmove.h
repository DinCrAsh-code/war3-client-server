//============================================================================
//  The per-unit movement pathfinder (0x6F49Axxx - 0x6F49Exxx, plus the
//  wavefront expanders at 0x6F4A1Exx - 0x6F4A2Fxx).
//
//  This is *not* the nearest-free-cell placement search in pathfind.h.  That
//  one answers "where can this footprint stand"; this one answers "which way
//  should this unit step next", and it is re-run continuously while the unit
//  is walking rather than once when the order is issued.
//
//  The shape of it, outermost first:
//
//    CPathClient::Advance          (0x6F49E020)  per-tick entry: a cooldown
//                                                counter, then a cheap
//                                                straight-line test, then
//                                                the search proper;
//    CPathClient::RunSearch        (0x6F49DF00)  three-way classifier -
//                                                cached point / blocked /
//                                                full search;
//    CPathClient::CoarseSearch     (0x6F49D8D0)  masked wavefront flood over
//                                                the region-portal graph
//                                                layered on the cell grid;
//    CPathClient::FinishSearch     (0x6F49DE90)  snap the node the flood
//                                                picked to a concrete cell,
//                                                or schedule a retry.
//
//  Two things are worth knowing before reading any of it.
//
//  There is no A*: no g/h/f score is computed anywhere in the tree, the
//  frontier is a flat array with no cost ordering, and there is no parent
//  chain to walk back.  The flood keeps a single "closest node to the goal"
//  candidate and that is the whole result.
//
//  And the collision mask is staged, not passed.  CPathClient::m_typeMask
//  (+0x9C) is copied onto the global CPathSearch's own m_typeMask (+0xA4)
//  once per search, and every cell test downstream reads it from there.
//  That is why the mask dword is touched exactly once per search from three
//  different addresses in the shipped binary and never appears in an
//  argument list.
//============================================================================
#ifndef PATHMOVE_H
#define PATHMOVE_H

#include "game.h"
#include "pathfind.h"
#include "spatialgrid.h"
#include "cellbuffer.h"

class CPathClient;

//----------------------------------------------------------------------------
//  One of the two point caches a client carries: a flat array of grid points
//  and how many of them are live.  The pair sits at +0x40 and +0x60 of the
//  client, 0x20 bytes apart, and the shipped code reaches them by index off
//  bases that differ per call site - see the comments on the accessors.
//----------------------------------------------------------------------------
class SPathPointCache : public CMemBlock
{
public:
    //  0x6F498770 - grow by `count` elements, then - unless `fill` is null -
    //  fill every new slot from that one repeated CGridVec2. Same shape as
    //  SPathFrontier::Grow (pathfrontierarray.cpp), an 8-byte element
    //  instead of a 4-byte pointer.
    //
    //  `fill` first, `count` second: the shipped function reads `count` out
    //  of its *second* stack argument slot ([esp+arg_4]) and its callers
    //  push the pointer first, the same way round SPathFrontier::Grow
    //  already had it.  An earlier pass here declared the pair the other way
    //  about, and nothing in this repo could see it: verify.py sees only two
    //  swapped displacements in an otherwise-clean stream, both ABI audits
    //  count argument *bytes* and two dwords are eight either way, and the
    //  linker cannot tell two ints from a pointer and an int when there is
    //  only one declaration.  Live, with the callee unhooked, `count` would
    //  have arrived as a pointer.
    int Grow(const CGridVec2* fill, unsigned int count);

    //  0x6F4981F0 - fill [start, min(start+count, m_count)) from one
    //  repeated CGridVec2 - see SPathFrontier::FillFrom for the same shape.
    void FillFrom(unsigned int start, const CGridVec2* value,
                  unsigned int count);

    //  0x6F4987F0 - shift-erase [start, start+count) out of the array.
    int EraseRange(unsigned int start, unsigned int count);

    CGridVec2* Points() { return (CGridVec2*)m_data; }

    unsigned int m_growChunk;    // +0x14
    unsigned int m_capacity;     // +0x18
    unsigned int m_count;        // +0x1C
};

//----------------------------------------------------------------------------
//  The per-search working state hanging off the global CPathSearch.  Only the
//  three fields this tree writes are named; the object is the same one
//  pathfind.h describes, reached through the same +0xA4 mask slot.
//----------------------------------------------------------------------------

//  m_flags bits this tree touches.
const unsigned int kPathFlagValid     = 0x00100000;  //  a route is current
const unsigned int kPathFlagMoved     = 0x10000000;  //  the point changed
const unsigned int kPathFlagSearched  = 0x02000000;  //  ran a search this tick
const unsigned int kPathFlagLaneHigh  = 0x04000000;  //  picks the far lane


//----------------------------------------------------------------------------
//  0x6FA84FF8 - the formation-lane table.  Sixteen sets of four lanes, each
//  lane 0x1C bytes; CPathClient::Lane picks the set with the four flag bits
//  at m_flags >> 16 and the lane within it from two more flags and the
//  client's own key.
//----------------------------------------------------------------------------
struct SPathLane
{
    //  --- 0x6F49AFA0 / 0x6F49B830 / 0x6F49B890 ------------------------
    void Unlink(CPathClient* client);
    void Link(CPathClient* client);
    int  Join(CPathClient* client);

    unsigned short m_key;           // +0x00  compared against m_laneKey
    char           m_reserved02[0x04 - 0x02];
    unsigned int   m_limit;         // +0x04
    unsigned int   m_pending;       // +0x08
    char           m_reserved0C[0x10 - 0x0C];
    int            m_members;       // +0x10  clients on the list below
    CPathClient*   m_head;          // +0x14
    CPathClient*   m_tail;          // +0x18
};

struct SPathLaneSet
{
    SPathLane m_lane[4];            // 0x70 bytes
};

extern SPathLaneSet g_pathLaneTable[];

//----------------------------------------------------------------------------
//  The movement client itself, cached at unit+0xA8.
//
//  Several offsets are genuinely dual-use - the cursors at +0x74/+0x78 land
//  inside the 0x20-byte stride of the second point cache, and +0x7C/+0x80 are
//  both the second cache's tail and the pair of per-movement-type recompute
//  stamps CPathClient::RateLimit indexes.  The shipped code reinterprets them
//  by call site, so the accessors below do too rather than pretending a
//  single field layout covers both readings.
//----------------------------------------------------------------------------
class CPathClient
{
public:
    //  ---- NIpse::CLrPath vtable slots (batch E, this session) --------
    //  RTTI-confirmed identity: `agent_worktrees/classes/0x6F95336C.json`
    //  (NIpse::CLrPath, 17 slots) gives slot 0's own target as 0x6F49CB60,
    //  which is a one-instruction `mov eax, offset aLrpath; retn` - the
    //  literal string "LrPath". Every field offset CLrPath::Save/Load
    //  below touches (m_position/m_target/m_goal, m_cache[2], m_cursor/
    //  m_stamp, m_sizeClass/m_laneKey, m_flags, m_lanePrev/m_laneNext,
    //  m_cooldown/m_retries, m_typeMask, m_ignore/m_targetNode,
    //  m_bestHandle/m_bestTypeTag, m_radius) already has a name in this
    //  class, confirming CPathClient - reconstructed here long before this
    //  vtable was visible (see CLAUDE.md's own "Caveat for classes/") - IS
    //  NIpse::CLrPath, not a merely similar sibling. Left named CPathClient
    //  rather than renamed: every one of this class's ~30 already-scoring
    //  methods and every caller elsewhere in this tree spells it that way,
    //  and a rename here is a much larger, separate-session change (see
    //  this batch's own write-up) - documented, not silently worked around.
    //
    //  Slot 0 (GetTypeName, 0x6F49CB60) is a free function below, matching
    //  NIpse::CRlAgent_GetTypeName's own shape (Pathfinding/crlagent.h) -
    //  `this` is never read.
    //
    //  Slot 1 (0x6F49CD70) / slot 2 (0x6F49CDB0):
    ~CPathClient();                              // 0x6F49CD10 (real dtor, lrpath_dtor.cpp)
    void ReturnToStaticPool();                   // 0x6F49CD70
    void* ScalarDeletingDestructor(unsigned int flags); // 0x6F49CDB0

    //  Slot 3 (0x6F49B680) - CPresenceTagged overrides slot 3 first
    //  (SHandleTableEntry::RegisterOrReuse, 0x6F4AEFF0, handletable.h/.cpp -
    //  same base-chain slot-3 column CRlAgent::Apply's own file header
    //  walks, Pathfinding/crlagent.h) - CLrPath calls straight into it
    //  (bypassing the vtable, matching the dump's own direct `call`) and
    //  then stamps its own added fields.  See pathclient_apply.cpp.
    void Apply(struct SHandleRegistrationRequest* request);

    //  Slot 4 (0x6F49DE40) - tear the search down (ResetSearch(-1,1,1,1)),
    //  clear both m_stamp entries (ResetStampsFrom(-1), 0x6F49B620, new
    //  below), zero m_targetNode/m_ignore, then unregister and notify
    //  (UnregisterAndNotify, 0x6F4AEF00, handletable.h - already
    //  reconstructed). See pathclient_slot4.cpp.
    void CancelAndUnregister(int arg);

    //  0x6F49B620 - zero m_stamp[start..end) where start/end come from
    //  `kind` the same way ResetSearch's own +0x74 clamp does (kind==-1
    //  means "the whole array"; ResetSearch's own dump-confirmed idiom,
    //  `sub/neg/sbb/and` folding "kind==-1 ? 0 : kind" into one branchless
    //  expression - not spelled out that way here, so this is filed
    //  DIFFERS/pending-angr rather than claimed EXACT by construction).
    //  Own TU (pathclient_resetstampsfrom.cpp) since it is reached by a
    //  real out-of-line call from Slot4/CancelAndUnregister.
    void ResetStampsFrom(int kind);

    //  Slot 9 (0x6F49D450) / slot 10 (0x6F49D5F0) - serialize every field
    //  this class owns through a CDataStoreScratch/CDataStore, in
    //  declaration order. See pathclient_save.cpp/pathclient_load.cpp.
    void Save(class CDataStoreScratch* store);
    void Load(class CDataStore* store);

    //  Slot 15 (0x6F49CDE0) / slot 16 (0x6F49CB70) - ComputeChecksum/
    //  DumpDebugState.  See pathclient_checksum_dump.cpp's own doc comment.
    void ComputeChecksum(unsigned int* hash);
    void DumpDebugState(void* sink);

    //  --- 0x6F49E020 --------------------------------------------------
    //  `out` and `mover` are threaded straight through to RunSearch's own
    //  `out` and StageSearch's own `mover` - confirmed by the call sites,
    //  not guessed; an earlier pass had these as `int limit, int tick`.
    int Advance(const CGridVec2* to, CGridVec2* out, CPathTrace* mover);

    //  --- 0x6F49BD10 --------------------------------------------------
    //  March a straight segment cell by cell and ask the shared
    //  CPathSearch whether the footprint fits at every cell the march
    //  enters.  `limit` is a CFloat *by value* - the shipped code `fld`s
    //  it straight out of its own argument slot.  Returns 1 if the whole
    //  segment is walkable, 0 the moment a cell is not.
    //
    //  `this` is never read: the search it asks is the global
    //  g_pPathSearch, not this client's own anything.  It is still a
    //  member rather than a free function because the shipped caller
    //  (0x6F49BEE0) loads ecx before the call, which nothing but a
    //  thiscall explains.
    int TestSegmentWalkable(const CGridVec2* from, const CGridVec2* step,
                            CFloat limit);

    //  --- 0x6F49B9C0 --------------------------------------------------
    //  One whole query pass: rate-limit it, take a lane, pin the two
    //  registrations that must not move under it, run the query on the
    //  shared CPathSearch, then republish the first point it produced as
    //  this client's own target - unless that point is already `dest`, in
    //  which case the "target moved" flag is cleared instead.
    int RunPathQuery(void* context, const CGridVec2* dest, int budget);

    //  --- 0x6F49DF00 / 0x6F49DFB0 -------------------------------------
    int RunSearch(const CGridVec2* to, CGridVec2* out);
    void SetRoute(const CGridVec2* pos, int haveGoal);

    //  --- 0x6F49DE90 --------------------------------------------------
    int FinishSearch(const CGridVec2* to);

    //  --- 0x6F49DAC0 / 0x6F49DBE0 -------------------------------------
    int TryStraightLine(const CGridVec2* to);
    void ResetSearch(int keepCache, int reserve, int release, int reset);

    //  --- 0x6F49D8D0 --------------------------------------------------
    int CoarseSearch(const CGridVec2* from, const CGridVec2* to);

    //  --- 0x6F49C930 / 0x6F49C990 -------------------------------------
    int NextCachedPoint(const CGridVec2* to, CGridVec2* out);
    int RetireCachedPoint(const CGridVec2* to, int force);

    //  --- 0x6F49C380 / 0x6F49C3B0 / 0x6F49C460 ------------------------
    void SeekCachedPoint(const CGridVec2* to, CGridVec2* out);
    //  `from` is written through when the cached run has to be re-seeked -
    //  the caller's own local, not a const in-only point.
    int Classify(const CGridVec2* to, CGridVec2* from);
    int SnapToCell(const CGridVec2* to, CGridRegistration* node);

    //  --- 0x6F49C100 / 0x6F49C2E0 -------------------------------------
    int TrimTail(int* outFlag, int keepLast);
    int PopWaypoint(int allowStep, int* movedOut);

    //  --- 0x6F49C5B0 --------------------------------------------------
    //  Returns `out`.  Both of the shipped body's exits already leave the
    //  out pointer in eax (they load it there to store through), and
    //  TryStraightLine reads it back - it keeps the returned pointer in edi
    //  and reaches the point through it rather than through its own local's
    //  address, which only a returning signature reproduces.
    CGridVec2* PointForSlot(CGridVec2* out, int slot);

    //  0x6F49C8D0 - not yet reconstructed: indexes both this+0x74 (the
    //  same array PointForSlot's own slot+1 lookup reaches into, beyond
    //  the two named m_cursor slots) and a second, 32-byte-stride array
    //  at this+(slot+2)*32, then compares a CFloatDistanceSquared result
    //  against `*threshold * scale`. Declared so RetireCachedPoint
    //  compiles; the two arrays it reads are the same not-yet-identified
    //  per-slot history PointForSlot needs.
    int CheckSlotDistance(const CGridVec2* to, int slot, const CFloat* scale);

    //  --- 0x6F49BB90 / 0x6F49BD10 / 0x6F49BEE0 ------------------------
    int RunTrace(const CGridVec2* from, const CGridVec2* to);
    int WalkSegment(const CGridVec2* from, const CGridVec2* step, float len);
    int FirstBlockedPoint(const CGridVec2* to);

    //  --- 0x6F49B2F0 --------------------------------------------------
    //  Which formation lane this client belongs to.  Defined here rather
    //  than in a .cpp because the shipped build has it both inlined into
    //  ReleaseLane and standing on its own at 0x6F49B2F0 - which is what an
    //  in-class definition produces: inlined where it fits, plus one
    //  out-of-line COMDAT for the call sites that take it as a call.
    SPathLane* Lane() const
    {
        SPathLane* lanes = g_pathLaneTable[(m_flags >> 16) & 0xF].m_lane;
        if ((m_flags & kPathFlagSearched) != 0)
            return &lanes[3];
        if (m_laneKey <= lanes[2].m_key)
            return &lanes[2];
        if ((m_flags & kPathFlagLaneHigh) != 0)
            return &lanes[1];
        return &lanes[0];
    }

    //  --- 0x6F49B340 / 0x6F49B3E0 -------------------------------------
    void ScheduleRetry(const CGridVec2* to);
    void ClearSearchState();

    //  --- 0x6F49B400 / 0x6F49B430 -------------------------------------
    void RewindCursor(int slot);
    void SetBestNode(const CGridRegistration* node, unsigned int cooldown);

    //  --- 0x6F49B470 --------------------------------------------------
    int SameCellRecently(const CGridVec2* pos, int shift, int* sameOut);

    //  --- 0x6F49B5A0 / 0x6F49B8E0 / 0x6F49B920 / 0x6F49B950 -----------
    void ReleaseLane();
    void StageSearch(CPathTrace* mover);
    unsigned char TailWaypoint();
    int PlaceWaypoint(const CGridVec2* pos);

    //  --- 0x6F49C690 ----------------------------------------------------
    //  Re-key which lane-table group Lane() picks from.
    void SetLaneTableIndex(unsigned char index);

    //  --- 0x6F49D2F0 --------------------------------------------------
    CHandleObject* ResolveBestNode() const;

    //  --- 0x6F49D770 --------------------------------------------------
    void TrackBest(SPathFrontier* frontier);

    //  --- 0x6F493E20 --------------------------------------------------
    //  Stage a movement type record's mask and ignore-registration onto
    //  this client, or park both at -1 when there is none.
    void SetTypeRecord(const void* record);

    //  --- 0x6F49ACA0 / 0x6F49AD10 / 0x6F49AD30 / 0x6F49AD40 -----------
    int RateLimit(int kind, unsigned int now, unsigned int* ageOut);

    //  0x6F49AD10 - see pathclientstate.cpp.  The mask arrives as a
    //  pointer and is copied, the same shape CPathSearch::TestFootprint
    //  (pathfootprintsize.cpp) already uses for its own +0xA4 twin.
    void SetTypeMask(const int* mask);

    void SetTargetNode(CGridRegistration* node);
    void SetRadius(const CFloat* radius);

    //----------------------------------------------------------------
    //  The two point caches sit back to back with no gap (RewindCursor,
    //  0x6F49B400, reads their counts at +0x50/+0x70, i.e. CMemBlock+0x1C
    //  from a base of +0x34/+0x54 - not the +0x40/+0x60 an earlier pass
    //  through this file guessed from TryStraightLine's own `esi+0x54`
    //  reference alone), so both are one array.  The cursor/stamp pairs
    //  immediately follow the second cache's own 0x20 bytes, ending
    //  exactly where it does.
    //----------------------------------------------------------------
    char               m_reserved00[0x14];
    unsigned int       m_tagT;          // +0x14  CPresenceTagged's own tag
                                         //        pair (pathclient_checksum_dump.cpp)
    unsigned int       m_tagB;          // +0x18
    CGridVec2          m_position;      // +0x1C  where the unit is now
    CGridVec2          m_target;        // +0x24  the point being walked to
    CGridVec2          m_goal;          // +0x2C  the order's own destination
    SPathPointCache    m_cache[2];      // +0x34 / +0x54
    unsigned int       m_cursor[2];     // +0x74 / +0x78
    unsigned int       m_stamp[2];      // +0x7C / +0x80
    unsigned short     m_sizeClass;     // +0x84
    unsigned short     m_laneKey;       // +0x86
    unsigned int       m_flags;         // +0x88
    //  The lane list is threaded through the clients themselves, with -1
    //  as the end sentinel and 0 meaning "not on any lane" - see
    //  pathlaneunlink.cpp.
    CPathClient*       m_lanePrev;      // +0x8C
    CPathClient*       m_laneNext;      // +0x90
    unsigned int       m_cooldown;      // +0x94  ticks left before a repath
    int                m_retries;       // +0x98  attempts left this order
    int                m_typeMask;      // +0x9C  staged onto the search
    CGridRegistration* m_ignore;        // +0xA0  the mover's own registration
    CGridRegistration* m_targetNode;    // +0xA4  the region node aimed at
    unsigned int       m_bestHandle;    // +0xA8  handle of the best node
    int                m_bestTypeTag;   // +0xAC
    char               m_reservedB0[0xB4 - 0xB0];
    CFloat             m_radius;        // +0xB4
};

//----------------------------------------------------------------------------
//  0x6FAB7750 / 0x6FAB7754 / 0x6FAB7758 / 0x6FAB774C - the four globals
//  CPathClient::StageSearch latches once per search so that the whole tree
//  below it can read them without threading four more arguments through.
//----------------------------------------------------------------------------
extern CPathSearch* g_pPathSearch;      // 0x6FAB7750
extern CCellGrid*   g_pPathGrid;        // 0x6FAB7754
extern CPathTrace*  g_pPathMover;       // 0x6FAB7758
extern short        g_pathQuality;      // 0x6FAB774C

//  0x6F953340 - one recompute interval per movement type, indexed by the
//  same `kind` CPathClient::RateLimit takes.
extern const unsigned int g_repathInterval[];


//  Encoded constants this module reads.
extern const CFloat g_pathSpanEpsilon;      // 0x6FAB7748
extern const CFloat g_pathCloseEnough;      // 0x6FAB775C
extern const CFloat g_pathTrimEpsilon;      // 0x6FAB7760
extern const CFloat g_pathStepScale;        // 0x6FAAE4CC
extern const CFloat g_slopeThreshold1;      // 0x6FAAE4F4
extern const CFloat g_CFloatPi;             // 0x6FAAE600

//  0x6F481450 / 0x6F481460 reached through the time-sync singleton, which is
//  all these two thunks do.
CPathSearch* PathSearchSingleton();     // 0x6F49B580
CPathSearch* PathOwnerSingleton();      // 0x6F49B590

//  0x6F49CB60 - NIpse::CLrPath's own vtable slot 0.  `this` never read;
//  returns the literal string "LrPath" (aLrpath) - see the RTTI note on
//  CPathClient's own class comment above.
const char* PathClient_GetTypeName();

//  0x6F38ADA0 - pathgridpoint.cpp's size classifier, reused here.
int __fastcall ClassifySlopeAxis(const CFloat* value);

//  0x6F6EF750 - square root of an encoded float.
CFloat __fastcall CFloatSqrt(const CFloat& value);

//  0x6F6EED10 - the restoring integer square root CFloatSqrt runs on the
//  normalised mantissa.
unsigned int __fastcall IntegerSqrt(unsigned int unused, unsigned int value);

//  0x6F6EF3B0 - squared distance between two grid points.  Not yet
//  reconstructed (a 325-instruction hand-fused subtract/square/add, not
//  composable from the ordinary CFloat operators) - declared so its
//  callers compile; left for a future session.
CFloat __fastcall CFloatDistanceSquared(const CGridVec2& a,
                                        const CGridVec2& b);

//  0x6F49B050 - normalize a direction vector in place, but only if it is
//  long enough to divide by safely; always returns its length.
CFloat NormalizeVec2(CGridVec2* pt);

//  0x6F2BAE80 - scale a grid vector by a scalar. Spelled out rather than
//  game.h's CGridVec2::operator*(CGridVec2,CFloat): this call site hands
//  both multiplies the same `scale` object, where operator* copies it once
//  per component (see timesyncphysics.cpp's RecomputeOrigin for the same
//  distinction).
CGridVec2 __fastcall ScaleGridVec2(const CGridVec2& v, const CFloat& scale);

//  0x6F49ABB0 - the direction bits between two cell points, in the same
//  encoding pathfind.h's kPathDir* constants use.  Takes both operands in
//  esi/edi, which no MSVC parameter list can ask for; declared __fastcall
//  because that is the only convention that puts *nothing* on the stack,
//  which is what every shipped call site does - it loads esi/edi itself
//  and calls with an empty argument area.  Under __cdecl this build pushed
//  two pointers and cleaned them again, three instructions the shipped
//  code does not have at any call site.  The registers are still the wrong
//  pair (ecx/edx rather than esi/edi) and the naked body still reads
//  esi/edi, so the arguments named here remain decorative either way -
//  see pathdirection.cpp.
int __fastcall DirectionBetween(const int* from, const int* to);

#endif
