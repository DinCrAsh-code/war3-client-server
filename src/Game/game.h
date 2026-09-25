//============================================================================
//  Types touched by CPathRef::SmartPtrToWorldVec3 (0x6F4743A0) and its callees.
//
//  Only the members this call tree actually reads are named; everything else
//  is kept as an explicit reserved block so that every named member lands on
//  the byte offset the disassembly uses.  The offsets are recoverable from
//  this call tree alone - the surrounding fields are not, so they are not
//  invented here.
//============================================================================
#ifndef GAME_H
#define GAME_H

#include "CFloat.h"
#include "agiletype.h"

struct SCellBox;   // boxmath.h - PinRegistration below takes one

class CGridRegistration;   //  spatialgrid.h - the grid side of a CPathTrace
class CPathSearch;         //  pathfind.h    - the nearest-free-cell search
class CCellGrid;           //  spatialgrid.h - the cell grid registrations live in
struct SAgileAgent;        //  agentquery.h  - the agent a CPathTrace moves
class AgileAgentQuery;     //  agentquery.h  - one borrowed enumeration scratch

//----------------------------------------------------------------------------
//  Anything reachable through the global handle table.  Fixed at exactly
//  0x1C bytes: CPathTrace (below) derives from this at that offset, so
//  anything a *specific* handle-carrying type adds beyond m_typeTag must
//  go on a further-derived struct (SItemHandleObject, right below) rather
//  than here, or it silently shifts every CPathTrace field by however many
//  bytes get added - which is exactly what happened here once already
//  (see docs/targets/JASS_SetItemPosition.md).
//----------------------------------------------------------------------------
class CHandleObject
{
public:
    char            m_reserved00[0x0C];
    int             m_kindTag;          // 0x0C  a second, finer-grained type
                                         //       tag some callers check
                                         //       against a fixed FourCC
                                         //       (0x2B61676C) even after the
                                         //       handle table's own m_typeTag
                                         //       already matched - see
                                         //       jasssetitemposition_native.cpp
    char            m_reserved10[0x14 - 0x10];
    unsigned int    m_flags;            // 0x14  bit 31 picks the sync channel
    int             m_typeTag;          // 0x18  checked against the handle's tag
};

//----------------------------------------------------------------------------
//  A CHandleObject as read by the item-handle resolution chain
//  (itemhandlemain.cpp/itemhandleresolve.cpp/jasssetitemposition_native.cpp):
//  the same header, plus two more fields those three call sites read that
//  are *not* part of the shared base every handle type derives from -
//  CPathTrace derives from CHandleObject at exactly 0x1C bytes (above), so
//  extending CHandleObject itself with these would silently shift every
//  CPathTrace field instead. Which concrete type actually carries them is
//  not established by this call tree; this is only the two fields the item
//  chain reads, at their real offsets, on whatever LookupHandle() actually
//  returns for an item handle.
//----------------------------------------------------------------------------
struct SItemHandleObject : public CHandleObject
{
    char            m_reserved1C[0x20 - 0x1C];
    int             m_pendingFlag;      // 0x20  nonzero blocks several
                                         //       accessors from resolving
                                         //       further - not otherwise
                                         //       identified
    char            m_reserved24[0x54 - 0x24];
    void*           m_field54;          // 0x54  read by
                                         //       jasssetitemposition_native.cpp's
                                         //       own sub_6F4786B0; not
                                         //       otherwise identified
};

//----------------------------------------------------------------------------
//  The handle table itself (g_pHandleTable, 0x6FAB7788): two parallel slot
//  arrays, picked by the handle's top bit.  A slot is live when its state
//  word is -2.
//
//  The full struct (SHandleSlot/SHandleTable/g_pHandleTable's own extern)
//  used to be modelled here independently of Pathfinding/handletable.h's
//  own SHandleTable - the same global, reconstructed twice by two sessions
//  that never saw each other's header, with two different-looking (but, on
//  inspection, byte-compatible) shapes: this file named the two slot arrays
//  directly, handletable.h derives the same bytes from
//  Math/indexedfreelist.h's STwoIndexedFreeLists (a positive-pool
//  SIndexedFreeListOwner at +0, a named m_negativeOwner at +0x20, each an
//  {records ptr @ +0x0C, capacity @ +0x1C} pair - which is exactly this
//  struct's own m_slots@0x0C/m_slotCount@0x1C and m_altSlots@0x2C/
//  m_altSlotCount@0x3C, just described via the allocator's own field names
//  instead of this table's). Two full definitions of the same type name is
//  a hard redefinition error the moment one translation unit includes both
//  headers (src/Agent/agentbaseabseventbinding.cpp's own header comment
//  worked around exactly this by declining to #include handletable.h at
//  all). handletable.h's version is kept as the one definition - it is
//  also what SHandleTableEntry::RegisterOrReuse's real AllocateAt/Push
//  calls need (STwoIndexedFreeLists member functions, implemented once at
//  their own real addresses; this struct having its own competing field
//  layout would have needed them re-implemented under a different class
//  name for no reason) - and LookupHandle (src/Agent/handle.cpp) reads the
//  same bytes through SIndexedFreeListOwner's own named fields instead of
//  m_slots/m_slotCount/m_altSlots/m_altSlotCount.  See handletable.h for
//  the struct itself.
//----------------------------------------------------------------------------
CHandleObject* __fastcall LookupHandle(unsigned int handle, int typeTag);

//----------------------------------------------------------------------------
//  A position/time pair sampled from one clock, plus the rate used to carry it
//  forward to another point in time.
//----------------------------------------------------------------------------
//  What CSyncChannel::Register hands back and CAgentRelation parks in its
//  own +0x1C/+0x20.  Only its flags word is ever touched from here.
//  Every field is one CSyncChannel::MakeRequest (src/Sync/syncchannelrequest.cpp)
//  writes; the record is what that function builds and hands back, and the
//  shipped RTTI descriptor its pool is allocated under is
//  `.?AV?$CSiRequest@VCPrRelation@NIpse@@J@NIpse@@` - i.e.
//  `NIpse::CSiRequest<NIpse::CPrRelation, long>`, the shipped names for
//  this record and for CAgentRelation.  The established names are kept.
class CSyncChannel;

struct SSyncRegistration
{
    void*         m_next;       // +0x00  always 0 at creation
    CFloat        m_deadline;   // +0x04  m_value + the clamped sample
    //  +0x08  the *clamped sample itself*, by value - not a pointer to the
    //  caller's own CFloat.  CSyncChannel::Register overwrites its own
    //  `value` argument slot with `*value` (0x6F4A66EA), clamps that slot up
    //  to g_CFloatEpsilon in place, and then passes the slot to MakeRequest
    //  with the `push ecx / mov eax,esp / mov [eax],ecx` copy-construct-into-
    //  the-argument-slot idiom a 4-byte class with a copy constructor gets.
    //  Recorded as a pointer here until Register was reconstructed; the two
    //  spellings emit the identical dword store, which is why nothing caught
    //  it earlier.
    CFloat        m_value;
    CSyncChannel* m_channel;    // +0x0C  the channel this is queued on
    unsigned int  m_flags;      // +0x10  bit 0: sample me; 0x10000: retired
    int           m_sequence;   // +0x14  the channel's own sample counter
    void*         m_owner;      // +0x18  who registered
    int           m_time;       // +0x1C  *the caller's time
};

class CSyncChannel
{
public:
    CFloat Extrapolate(const CFloat& value, int time,
                       const CFloat& base, int baseTime) const;

    //  0x6F4A66E0 - take a new sample on this channel: clamp `value` up to
    //  g_CFloatEpsilon if it is below it, bump the sample counter, and hand
    //  the counter, the clamped value and m_value + the clamped value on to
    //  the real recorder.  Returns the registration the caller stores.
    //  Not reconstructed - it is above this dump's own root - so it is a
    //  thunk in agentrelation_thunks.cpp; `retn 0Ch`.
    void* Register(void* owner, int* time, const CFloat* value);

    //  0x6F4A6650 - the real recorder Register() hands on to: take a
    //  request record off this channel's own free list (or its pool), fill
    //  it in and push it onto the channel's deadline heap.  `retn 14h`.
    //  The channel's own sub-objects are reached through an overlay rather
    //  than by growing m_reserved00 - see src/Sync/syncrequest.h.
    SSyncRegistration* MakeRequest(void* owner, int* time,
                                   CFloat value,
                                   const CFloat& deadline, int seq);

    char            m_reserved00[0x40];
    CFloat          m_value;            // 0x40
    int             m_time;             // 0x44
    CFloat          m_rate;             // 0x48
    char            m_reserved4C[0x04];
    //  +0x50 - a monotonically increasing sample counter Register() bumps
    //  once per call and passes on as the sample's own sequence number.
    int             m_sampleSeq;        // 0x50
};

class CTimeSync
{
public:
    CFloat Extrapolate(const CHandleObject* object,
                       const CFloat& value, int time) const;
    const CSyncChannel* GetChannel(const CHandleObject* object) const;

    //  0x6F483430 - read the object's channel sample out into the caller's
    //  own copy of it, e.g. so a moved object's cached sync fields track the
    //  channel again after ApplyDelta touches them - see
    //  src/timesyncphysics.cpp.
    void GetChannelSample(const CHandleObject* object, CFloat* outValue,
                          int* outTime) const;

    //  0x6F481450 / 0x6F481460 - the two one-instruction accessors the
    //  placement search reaches this object for: the search state it runs
    //  on, and the object whose grid and settings it runs against.  Both
    //  live far past the sync channels; the gap between is not recoverable
    //  from this call tree, so it stays reserved.
    CPathSearch* GetPathSearch() const;
    CPathSearch* GetPathOwner() const;

    //  0x6F481420 - the third one-instruction accessor on this object, and
    //  the one the agent enumerators (src/Agent/agentqueryenumerate.cpp)
    //  reach for the cell grid.  See timesynccellgrid.cpp.
    CCellGrid* GetCellGrid() const;

    //  0x6F488330 - clip a cell rectangle against the path grid's own
    //  extent and, if anything is left of it, run the pin (or release) over
    //  the four slots at +0x23C.  A null first argument means "the whole
    //  grid extent"; a non-null one is a cell rectangle to intersect with
    //  it - which is what a CGridRegistration's own +0x1C turns out to be,
    //  so the old "the registration's +0x1C sub-object" reading was right
    //  about the address and silent about the type.  Reconstructed in
    //  timesyncpinregistration.cpp.
    void PinRegistration(const SCellBox* clip, int pin);

    //  0x6F4883D0 - the float-rectangle front door to PinRegistration: turn
    //  an already cell-scaled float rectangle into the half-open cell
    //  rectangle covering it (through the path search's own grid) and pin
    //  that.  A null rectangle means "the whole grid extent" and is handed
    //  straight through, which is why the null case is a separate `retn 8`
    //  tail in the shipped code rather than a branch around the conversion.
    void PinFloatBox(const struct SCellBoxF* box, int pin);

    char            m_reserved00[0x14];
    CSyncChannel    m_localChannel;     // 0x14
    CSyncChannel    m_remoteChannel;    // 0x68
    char            m_reservedBC[0x234 - 0xBC];
    //  +0x234 - the cell grid the agent enumerators register against and
    //  read m_cellScale out of.  Its own accessor rather than a bare field
    //  because the shipped code always reaches it through the out-of-line
    //  0x6F481420 below, three times in a row in some callers.
    CCellGrid*      m_cellGrid;         // 0x234
    char            m_reserved238[0x23C - 0x238];
    //  +0x23C - four consecutive dwords CTimeSync::PinRegistration walks as
    //  one array: it hands the first to the setup call and then steps a
    //  pointer through them, passing each adjacent *pair* to the follow-up.
    //  What they are is not established by that one function.
    unsigned int    m_pinSlots[4];      // 0x23C .. 0x248 (ends at 0x24C)
    CPathSearch*    m_pathSearch;       // 0x24C
    CPathSearch*    m_pathOwner;        // 0x250
    char            m_reserved254[0x538 - 0x254];
    //  0x538 - the current game tick, compared against a mover's own
    //  m_stamp[0]/m_stamp[1] by CPathClient::SameCellRecently (0x6F49B470)
    //  to decide whether a repeated cell hit is recent enough to matter.
    unsigned int    m_tick;             // 0x538
};

extern CTimeSync* g_pTimeSync;          // 0x6FAB73D8

//  0x6F496810 - a genuine free function (no `this`, no stack argument at
//  all): reads g_pTimeSync directly rather than accepting a receiver, so a
//  member declaration would force every call site to needlessly load `ecx`
//  first.  `+0x858` lands well past every CTimeSync field this call tree
//  otherwise reaches (the last is m_tick at +0x538) - plain pointer
//  arithmetic rather than a named member, since nothing here establishes
//  what object actually lives there.  Reached from Missile/
//  bhpoprojectile_attach.cpp's own allocation-lookup path.
void* GetTimeSyncField858();

//  0x6F4AAC00 - the `CBhPoBehavior` sibling of GetTimeSyncField858 above:
//  same shape, same global, `+0x838` instead of `+0x858`. Own translation
//  unit (Sync/ctimesyncfield838.cpp) for the same reason.
void* GetTimeSyncField838();

//----------------------------------------------------------------------------
//  Positions.  Grid space is the pathing grid; one grid unit is 32 world
//  units, and the grid's origin is stored in the shared grid description.
//----------------------------------------------------------------------------
class CGridVec2
{
public:
    CGridVec2() : m_x(), m_y() {}
    CGridVec2(const CGridVec2& other) : m_x(other.m_x), m_y(other.m_y) {}
    CGridVec2(const CFloat& x, const CFloat& y) : m_x(x), m_y(y) {}
    //  The same "leave it alone" tag CFloat carries, for a local the
    //  shipped code fills field by field without zeroing it first.
    CGridVec2(ECFloatNoInit) : m_x(kCFloatNoInit), m_y(kCFloatNoInit) {}

    //  0x6F07B870 - rescale this vector so that it is `length` long, by
    //  multiplying both components by length/|v|.  Unconditional: the one
    //  caller (CPathTrace::AddVelocity) has already established that |v|
    //  is over the cap it is passing.  See Math/gridvec2scaletolength.cpp.
    void ScaleToLength(const CFloat* length);

    CFloat m_x;
    CFloat m_y;
};

// PointAtDistance's shipped code copies the scalar once per component
// instead of handing both multiplies the same object, so the copies are
// spelled out here.  RecomputeOrigin does not - see timesyncphysics.cpp,
// which spells its own multiply out rather than using this operator.
inline CGridVec2 operator*(const CGridVec2& v, const CFloat& scale)
{
    return CGridVec2(v.m_x * CFloat(scale), v.m_y * CFloat(scale));
}

inline CGridVec2 operator+(const CGridVec2& a, const CGridVec2& b)
{
    return CGridVec2(a.m_x + b.m_x, a.m_y + b.m_y);
}

class CWorldVec3
{
public:
    CWorldVec3() { m_x = m_y = m_z = CFloat(); }
    CWorldVec3(const CWorldVec3& other)
        : m_x(other.m_x), m_y(other.m_y), m_z(other.m_z) {}
    CWorldVec3(const CFloat& x, const CFloat& y, const CFloat& z)
        : m_x(x), m_y(y), m_z(z) {}
    //  The same "leave it alone" tag CGridVec2 already carries, for a local
    //  the shipped code overwrites word for word before reading any of it -
    //  JASS_GetUnitLoc's own vector (jassnatives_locfactory.cpp).  Purely
    //  additive: nothing that does not ask for it changes.
    CWorldVec3(ECFloatNoInit)
        : m_x(kCFloatNoInit), m_y(kCFloatNoInit), m_z(kCFloatNoInit) {}

    //  0x6F0201B0 - Math/worldvec3ops.cpp.  `retn 4`, thiscall: `this` is
    //  the vector, the one stack argument is where the CFloat result goes.
    CFloat* __thiscall LengthSquared(CFloat* out) const;

    CFloat m_x;
    CFloat m_y;
    CFloat m_z;
};

//  0x6F020210 - Math/worldvec3ops.cpp.  `retn 4`, __fastcall: the hidden
//  return pointer takes ecx, `a` takes edx, `b` is the one stack argument -
//  see that file's own comment for how the call site pins down which
//  operand is which.
CWorldVec3 __fastcall operator-(const CWorldVec3& a, const CWorldVec3& b);

//  The singleton at 0x6FAB7368 is not just the grid description: the same
//  object carries the agile type registry at +0x0C, so it is the game's shared
//  static data rather than anything as narrow as an origin.  Only the members
//  two call trees have reached are named.
class CGameData
{
public:
    //  0x6F471840 - __fastcall member: `this` in ecx, the type in edx and the
    //  base on the stack.
    int __fastcall IsTypeDerivedFrom(unsigned int type, unsigned int base);

    char            m_reserved00[0x0C];
    AGILE_TYPE_TABLE m_types;           // 0x0C
    char            m_reserved34[0x20];
    //  0x54 - the switch IsQueryModeGateOpen (querymodegate.cpp) hands
    //  BuildAndRegisterEventBinding.  Named because that is the only thing
    //  that reads it; nothing here asserts what the value means.
    int             m_queryModeGate;    // 0x54
    //  0x58 - a stack of reusable AgileAgentQuery scratch objects, and
    //  0x64 how deep into it the enumeration currently is.  Every agent
    //  enumerator borrows m_queryPool[m_queryDepth++] on entry and gives
    //  it back on exit, growing the array by one whenever the depth has
    //  caught up with it - which is what makes a query run from inside a
    //  visit callback safe.  The array is a TSFixedArray, not a
    //  TSGrowableArray: m_queryDepth sits where the growable one's
    //  m_chunk would be.
    TSFixedArray<AgileAgentQuery*> m_queryPool;   // 0x58
    int             m_queryDepth;       // 0x64
    char            m_reserved68[0x6C - 0x68];
    CFloat          m_originX;          // 0x6C
    //  The high corner of the playable rectangle, named here now that
    //  FindFreeWorldPoint (0x6F473D10) clamps both axes of a whole
    //  rectangle against it.  gamebounds.h reached the same two fields
    //  through kGameDataBoundHiX/Y before the offsets were this well
    //  attested and still does - same bytes, same code.
    CFloat          m_originY;          // 0x70
    CFloat          m_boundX;           // 0x74
    CFloat          m_boundY;           // 0x78
    char            m_reserved7C[0x80 - 0x7C];
    //  0x80 - a global speed cap PositionTrack::SetMaxSpeed
    //  (Pathfinding/positiontrackspeed.cpp) clamps its own caller-supplied
    //  world-space speed down to before converting it to grid units and
    //  installing it on the resolved CPathTrace.  Only that one call tree
    //  reaches this offset, so nothing more specific than "a speed limit"
    //  is claimed for it.
    CFloat          m_globalMaxSpeed;   // 0x80
};

extern CGameData* g_pGameData;          // 0x6FAB7368

// One pathing grid cell is 32 world units: scale by adding 5 to the exponent,
// leaving an encoded zero alone.
inline CFloat GridToWorld(const CFloat& value)
{
    int bits = value.m_bits;
    return CFloat(bits + (((bits & kCFloatExpMask) != 0) ? 5 * kCFloatExpOne : 0));
}

//  ... and back: subtract the same five exponent steps, flushing to zero
//  when that would underflow the exponent field.  The underflow test is
//  made against *six* steps, not five: `bits ^ (bits - 6*step)` has its top
//  bit set exactly when taking six steps off would borrow out of the
//  exponent into the sign bit, which is one step of headroom, and the
//  arithmetic-shift-and-complement turns that into an all-ones keep mask.
//  Both spellings are the shipped ones - `bits - 5*step` is written as an
//  expression so it compiles to `add reg, 0FD800000h`.
inline CFloat WorldToGrid(const CFloat& value)
{
    int bits = value.m_bits;
    return CFloat((bits - 5 * kCFloatExpOne) &
                  ~((bits ^ (bits - 6 * kCFloatExpOne)) >> 31));
}

//----------------------------------------------------------------------------
//  A straight move in progress: where it started, which way it goes, and how
//  far along it was at the last synchronisation point.
//----------------------------------------------------------------------------
class CPathTrace : public CHandleObject
{
public:
    CFloat GetTravelDistance() const;
    CGridVec2 PointAtDistance(const CFloat& distance) const;

    //  0x6F38B140 - the point this trace is at right now: just
    //  `PointAtDistance(GetTravelDistance())`, in its own translation
    //  unit (Pathfinding/CPathTrace2.cpp) the same reason that file's own
    //  header already gives for AddOriginDelta - so the calls into both
    //  stay real calls rather than getting inlined into a caller under
    //  `/Ob2` (confirmed the hard way: this session tried it in
    //  CPathTrace.cpp first, where GetTravelDistance disappeared
    //  entirely, folded into an inlined CTimeSync::Extrapolate call).
    CGridVec2 GetCurrentPosition() const;

    //  0x6F4737D0 - fold a map-origin offset into the trace's current point
    //  and forward the result (plus a passthrough flag) to ApplyDelta.
    //  Reached from sub_6F474250, which is the only evidence for the
    //  signature; see src/timesyncbounds.cpp.
    void AddOriginDelta(const CGridVec2* originDelta, int flag);

    //  0x6F4A7380 - move the origin along the direction for the elapsed
    //  distance, add `delta`, then optionally (when `notify` is set) tell a
    //  virtual observer at vtable slot 0x54 about the new origin.
    void ApplyDelta(const CGridVec2* delta, int notify);

    //  0x6F4A7350 - replace m_radius and re-register both grid boxes for
    //  it, through RecomputeOrigin with a zero delta.
    void SetRadius(const CFloat* radius);

    //  0x6F4A70D0 - add `delta` to the velocity at +0x80, clamp the result
    //  to m_maxSpeed, flush it to zero if it came out under the module's
    //  tolerance, publish the resulting "is moving" bit onto the collision
    //  registration and - when `notify` is set - tell vtable slot 0x50.
    //  See Pathfinding/pathtraceaddvelocity.cpp.
    void AddVelocity(const CGridVec2* delta, int notify);

    //  0x6F4A73B0 - settle the position the old velocity has earned so far
    //  (RecomputeOrigin with a zero delta) and only then change the
    //  velocity by `delta`.  The order is the whole point of the function.
    //  See Pathfinding/pathtracepublishvelocity.cpp.
    void PublishVelocityDelta(const CGridVec2* delta, int notify);

    //  0x6F4A73F0 - replace m_maxSpeed outright (the caller has already
    //  clamped/converted it - see PositionTrack::SetMaxSpeed,
    //  Pathfinding/positiontrackspeed.cpp, the only caller in this repo),
    //  then re-run the current velocity through the same length-vs-cap test
    //  AddVelocity's own "over the cap" branch makes: if the velocity this
    //  trace already has now exceeds the *new* cap, force a re-clamp by
    //  republishing a zero delta (which still drives AddVelocity's own
    //  ScaleToLength). See Pathfinding/pathtracesetmaxspeed.cpp.
    void SetMaxSpeed(const CFloat* speed);

    //  0x6F4937B0 - raw-copy `value`'s own dword bits onto m_fieldB4.  The
    //  one caller in this repo is SmartPositionTrack::SetMaxSpeed
    //  (positiontrackspeed_smart.cpp), right after its own
    //  CPathTrace::SetMaxSpeed call, with the identical grid-unit CFloat
    //  pointer.  See Pathfinding/pathtracesetfieldb4.cpp and
    //  Pathfinding/smartpositiontrackspeed.cpp.
    void SetFieldB4(const CFloat* value);

    //  0x6F493A40 - the absolute form of the above: set the velocity *to*
    //  `velocity`, by publishing the difference from the one it already
    //  carries.  See Pathfinding/pathtracesetvelocity.cpp.
    void SetVelocity(const CGridVec2* velocity, int notify);

    //  0x6F4A6FD0 - the arithmetic ApplyDelta() forwards to: recompute the
    //  origin, refresh the sync sample, and re-register both grid boxes.
    void RecomputeOrigin(const CGridVec2* delta);

    //  0x6F497440 - re-read this trace's own sync fields from the global
    //  time sync channel, keeping them in step after a move.
    void RefreshSyncSample();

    //  0x6F4A6D70 - rebuild the pathing grid registration's cell rectangle
    //  from the new position and this trace's own radius, and hand it to
    //  m_pathGrid->UpdateBox().
    void UpdatePathGridBox(const CFloat* radius, const CGridVec2* pos);

    //  0x6F4A6E40 - the same for the collision grid, with the rectangle
    //  grown per the size class ClassifySlopeAxis() puts the radius in.
    void UpdateCollisionGridBox(const CFloat* radius, const CGridVec2* pos);

    //  0x6F495A00 - ask this trace to take one step to `pos`, returning
    //  non-zero when it may.  Not reconstructed: its shipped body carries a
    //  __CxxFrameHandler3 frame around a refcount it holds across the call,
    //  which this /EHs-c- toolchain never emits for any source shape (see
    //  docs/msvc-vc8-idioms.md).  retn 10h - four stack arguments.
    int TryStep(const CGridVec2* pos, int a, int b, int c);

    //  0x6F495940 - resolve the {handle,typeTag} pair below through
    //  LookupHandle. Reached from CPathClient::ScheduleRetry
    //  (pathscheduleretry.cpp), which reads the result's own +0x38 -
    //  CGridRegistration::m_visitStamp's own offset - so this is declared
    //  to return that type directly rather than the generic CHandleObject*
    //  LookupHandle itself returns.
    CGridRegistration* ResolveTargetHandle();

    //  0x6F477180 - the *other* reading of +0x78..+0x84.  A second family
    //  of methods on this same object treats the four CFloats not as two
    //  points but as one clamped linear ramp: value = base + rate * t,
    //  held between a low and a high bound.  The evidence that it really is
    //  this object and not a sibling class is 0x6F4A8FD0, which calls
    //  GetTravelDistance() and 0x6F477180 on the same `this`.  Spelled as
    //  accessors rather than as a union so that nothing already matching
    //  through m_origin/m_direction has to move - the same reason
    //  CGridRegistration's +0x34 carries two spellings.
    CFloat& RampBase()  { return m_origin.m_x; }     // +0x78
    CFloat& RampRate()  { return m_origin.m_y; }     // +0x7C
    CFloat& RampMin()   { return m_direction.m_x; }  // +0x80
    CFloat& RampMax()   { return m_direction.m_y; }  // +0x84

    //  0x6F477180 - evaluate that ramp at `t` and clamp it into
    //  [RampMin(), RampMax()], writing the answer through `out`.
    void RampValueAt(CFloat* out, const CFloat* t);

    //  0x6F4A8FD0 - where `t` sits in the ramp's own window, as a fraction:
    //  (t - RampValueAt(travelled)) / RampRate(), or -1 when `t` is outside
    //  [RampMin(), RampMax()] at all.
    void RampFractionAt(CFloat* out, const CFloat* t);

    //  0x6F4771E0 - the ramp evaluated at THIS trace's own current travel
    //  distance, right now, into the caller's buffer - the same
    //  GetTravelDistance()/RampValueAt() pair Unit/unittrackedref.cpp's own
    //  SUnitTrackedRef::GetValue already establishes for a resolved
    //  handle's own trace, just called directly on an already-resolved
    //  CPathTrace* instead of going through LookupHandle first. `out` is
    //  returned, which is what puts it in eax (the shipped `mov eax,edi`
    //  right before the epilogue).
    CFloat* RampValueAtCurrent(CFloat* out);

    char               m_reserved1C[0x30 - 0x1C];
    //  0x30 - the agile-agent registration this trace is the position of.
    //  Named by the agent enumerators (src/Agent/agentqueryenumerate.cpp),
    //  which reach a candidate's agent as `trace->m_owner->m_agent` and
    //  refuse the candidate when the same object's +0x20 is non-zero -
    //  the identical pair the agile type registry's own per-type instance
    //  walk (0x6F46D0B0) tests on the nodes of its list.
    SAgileAgent*       m_owner;          // 0x30
    char               m_reserved34[0x70 - 0x34];
    CFloat             m_syncDistance;   // 0x70
    int                m_syncTime;       // 0x74
    CGridVec2          m_origin;         // 0x78
    //  0x80 - read as a *velocity* by the three functions below as well as
    //  as a direction by RecomputeOrigin: the pair is added to, its length
    //  measured against m_maxSpeed and clamped to it, and zeroed outright
    //  when it comes out under the module's equality tolerance.  Same field,
    //  same four CFloats - `origin += direction * travelled` is exactly
    //  "position += velocity * elapsed" once m_direction is allowed to carry
    //  a magnitude, which is why nothing here is spelled as a second name.
    CGridVec2          m_direction;      // 0x80
    //  0x88 - the speed cap AddVelocity clamps |m_direction| to, and the
    //  same value whose sign decides the 0x20000000 "this registration is
    //  moving" bit on m_collisionGrid.
    CFloat             m_maxSpeed;       // 0x88
    //  0x8C - read (never written) by PositionTrack::GetField8C
    //  (positiontrackfield8c.cpp) through an out-pointer arg and handed
    //  straight back to its own caller; nothing in that call tree gives it
    //  a type beyond "one dword", so it stays generic per CLAUDE.md's
    //  "do not invent fields the call tree does not read".
    void*              m_field8C;        // 0x8C
    //  The radius the two grid registrations grow the position into a
    //  rectangle by.  A single CFloat, not a vector: 0x94 is a pointer.
    CFloat             m_radius;         // 0x90
    CGridRegistration* m_pathGrid;       // 0x94
    CGridRegistration* m_collisionGrid;  // 0x98
    unsigned int       m_targetHandle;   // 0x9C
    int                m_targetTypeTag;  // 0xA0
    char               m_reservedA4[0xB4 - 0xA4];
    //  0xB4 - written (never read in this call tree) by
    //  SmartPositionTrack::SetMaxSpeed's own extra call
    //  (Pathfinding/pathtracesetfieldb4.cpp), the same grid-unit CFloat it
    //  just installed as m_maxSpeed above, raw-copied by dword rather than
    //  through the CFloat copy assignment - a plain "one dword" field per
    //  CLAUDE.md's "do not invent fields the call tree does not read".
    unsigned int       m_fieldB4;        // 0xB4
    char               m_reservedB8[0xC8 - 0xB8];
    //  0xC8 - read (never written in this call tree) by
    //  SmartPositionTrack::GetFieldC8 (Pathfinding/smartpositiontrackfield8c.cpp)
    //  through an out-pointer arg and handed straight back to its own
    //  caller - the same shape PositionTrack::GetField8C has for its own
    //  +0x8C, just a different offset (confirmed off the raw disassembly:
    //  `mov ecx, [eax+0C8h]`, not +0x8C).  Nothing in this call tree gives
    //  it a type beyond "one dword", so it stays generic per CLAUDE.md's
    //  "do not invent fields the call tree does not read".
    void*              m_fieldC8;        // 0xC8
};

//----------------------------------------------------------------------------
//  A handle referring to a CPathTrace.
//----------------------------------------------------------------------------
//  Returned by value out of CPathRef::MakeMoveModifier below; defined in
//  positionmodifier.h, which includes this header, so it is only named
//  here.  A by-value return type may be incomplete in a declaration.
struct SMovementModifierRef;

class CPathRef
{
public:
    CWorldVec3 SmartPtrToWorldVec3() const;
    CWorldVec3& ToWorldVec3(CWorldVec3& out) const;

    //  0x6F4743C0 - publish a new world position on the object this ref
    //  names, and hand `this` back so the call reads as an assignment.
    //  Its whole body past the two pushes is the same 0x6F474250 that
    //  timesyncbounds.h reaches under the SHandleWithType name - the same
    //  sixteen-byte record seen from a second angle, which is a split this
    //  repo already had before this target and not one introduced here.
    CPathRef* SetWorldPosition(const CWorldVec3* pos);

    //  0x6F4741A0 - make a MovementModifier that walks this position to
    //  (x, y) at `rate`, and hand it back as a counted reference.  See
    //  src/Widget/positionmodifiermake.cpp.
    SMovementModifierRef MakeMoveModifier(const CFloat* x, const CFloat* y,
                                          const CFloat* rate);

    //  0x6F4758D0 - resolve this ref's own current world position, then
    //  marshal it (as a {position, self} pair) plus a {targetRef, selfRef}
    //  pair and a {targetField34, selfField34} pair into the real travel-
    //  cost search at 0x6F475560 (unreconstructed - see
    //  Pathfinding/pathrefestimatetravelcost.cpp's own comment).  Returns
    //  whatever that search returns in eax - an int cost/step count, judged
    //  from its one caller (Pathfinding/playerdistancescan.cpp, 0x6F2DEC20).
    int EstimateTravelCost(const CWorldVec3* targetPos, int range,
                           CPathRef* targetRef, int targetField34,
                           CPathRef* selfRef, int selfField34,
                           int flagA, int flagB) const;

    char            m_reserved00[0x08];
    unsigned int    m_handle;           // 0x08
    int             m_typeTag;          // 0x0C
};

#endif
