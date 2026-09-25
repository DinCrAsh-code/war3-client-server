//============================================================================
//  NIpse::CPoPoInterfLis (`??_7CPoPoInterfLis@NIpse@@6B@`, 0x6F952E74, 20
//  slots) - one of a sibling pair (with NIpse::CPoPoVelocityMod,
//  cpopovelocitymod.h) branching directly off NIpse::CPrRelation/
//  CAgentRelation (Agent/agentrelation.h), one level below the 19-slot
//  "Po-modifier/listener" cluster F family (Agent/cporethresholdlis.h and
//  siblings) - RTTI hierarchy confirmed
//  (agent_worktrees/classes/0x6F952E74.json):
//  "CPoPoInterfLis: NIpse::CPrRelation, NTempest::CPresenceTagged,
//  NTempest::CPresence, NTempest::CLinkedNode<NTempest::CPresence>;".
//
//  CPrRelation's own vtable already has 19 slots (0-18, see
//  agentrelation.h); this class's own vtable has 20 (0-19). Per this task's
//  own slot cross-reference against agent_worktrees/classes/0x6F952E74.json:
//
//    - slot 17 (+0x44, 0x6F47FFE0) - the shared no-op, inherited unchanged.
//    - slot 18 (+0x48, 0x6F4A69B0) - the SAME address as CPrRelation's own
//      slot 18, `CAgentRelation::OnRegistrationRetired` (agentrelation.h) -
//      confirmed by raw_asm read directly off agent_worktrees/funcs/
//      0x6F4A69B0.json: byte-identical body, inherited unmodified, not a
//      real override of this class.
//    - slot 19 (+0x4C, 0x6F506EA0) - CONFIRMED MISATTRIBUTED. Read directly
//      (worktree_store.py show 0x6F506EA0): an SEH-framed function that
//      stamps `??_7CGeoset@@6B@` and streams "HGEOSET"/"HGEOSETDATA"
//      handle-table pairs - a model-geoset-loading function with nothing to
//      do with this positioning/relation family, the exact same
//      bulk-dump-misattribution finding
//      docs/targets/NTempestNIpse_clusterH_remainder.md's own "idx23"
//      section already made for this identical address, one class removed.
//      Left untouched here too - not claimed, not attempted.
//
//  So this class's own genuinely new work is only slots 0-4, 8, 9, 10, 15,
//  16 - the same 9-position "own slot" pattern the 19-slot cluster-F family
//  documents (Agent/cporethresholdlis.h's own file header), PLUS slot 8,
//  which CPrRelation's own vtable does NOT define as anything but the
//  shared no-op `0x6F479900`-family placeholder at that position in the
//  17-slot CPresenceTagged base - this class's own slot 8 (0x6F4A8870) is a
//  real, different address, genuinely new here (confirmed by reading its
//  own raw_asm: a real '^sig' filter body, not a no-op).
//
//  Named from GetTypeName's own literal string, "PoPoInterfLis"
//  (aPopointerflis, immediately after this vtable's own RTTI data).
//
//  Fields (offsets confirmed from ComputeChecksum's/DumpDebugState's own
//  raw_asm, both of which read only these two - the class carries at least
//  one more field past +0x58, read by Apply/Slot4/Save/Load below, but this
//  session did not model it - see the "Not attempted" note):
//    m_radius (CFloat, +0x50) - a distance/radius threshold, from `arg`'s
//      own +0x0C field at Apply time (PoPoInterfLisQueryRadius,
//      cpopointerflis_apply.cpp), default g_CFloatZero.
//    m_opt    (unsigned int, +0x54) - always zeroed by Apply; printed as a
//      raw hex flags word ("opt[0x%X]") by DumpDebugState.
//
//  Resolved this follow-up session - the +0x58 array, slot 2's real dtor,
//  Slot4/Save/Load, and slot 8's own short dispatcher:
//
//    - The +0x58 container is a `CMemBlock`-backed growable array of
//      0x0C-byte records, header-shaped EXACTLY like
//      Missile/bhpoprojectile.h's own `SBhPoTickRelaySlotArray` (a
//      `CMemBlock`, then m_growBy/m_capacity/m_count - see
//      `SInterfLisEntryArray` below) but holding three-dword records
//      instead of raw pointers. Confirmed by `sub_6F4A7B10`'s/
//      `sub_6F4A7BA0`'s own per-element loops striding 0x0C bytes
//      (three `lea reg,[reg+reg*2]`/`add reg,reg` doublings = `*12`, the
//      same idiom `RemoveRange`'s own 4-byte version uses for `*4` in
//      bhpoprojectile_slots.cpp) and by `sub_6F4A7AB0`'s identical
//      `RemoveRange` shape at that same stride - this session's own
//      `SInterfLisEntryArray::RemoveRange` (cpopointerflis_array.cpp).
//      The three record fields themselves (`SInterfLisEntry` below) are
//      opaque past "three dwords written/read as plain WriteDword/
//      ReadDword calls, the last one through the Alt2/Alt3 pair" - this
//      session did not chase what they mean, only how they are
//      moved/stored/torn down (the same "generic array machinery, meaning
//      not chased" call this repo already made for `SBhPoTickRelaySlotArray`
//      itself).
//    - Slot 2 (0x6F48C9A0) - NOT the trivial restamp-only shape every
//      other class in this family uses: calls a real, SEH-framed
//      `~CPoPoInterfLis` (0x6F48C950) first (this class's own real
//      destructor, cpopointerflis_dtor.cpp), which tears down the +0x58
//      array via the already-real `ReleaseCMemBlockBase`
//      (Pathfinding/cellbuffer.h, `sub_6F4B3770`) before restamping to
//      `??_7CPresence@NTempest@@6B@` - the exact
//      `CPoPosBh::ScalarDeletingDestructor`/`~CPoPosBh` shape
//      (Agent/poposbh.h) reused here, same restamp target. The dtor's own
//      SEH frame (`__except_handler4`) is the same unreproducible-under-
//      this-build's-fixed-`/GS-`-`/EHs-c-` gap `poposbh_dtor.cpp`'s own
//      header already documents - the two real instructions it wraps (the
//      array release, the vtable restamp) are present and in order.
//    - Slot4 (0x6F4A8210) - if this object's own m_array.m_count is
//      non-zero, removes `m_array.m_count` elements from the front of the
//      array (`SInterfLisEntryArray::RemoveRange(0, m_array.m_count)`,
//      cpopointerflis_array.cpp - functionally a full clear, since
//      removing every currently-live element from index 0 is what a
//      `start=0, count=m_count` call does), sets the base's own
//      `CAgentRelation::m_flags` bit 0x1000000, then always tail-calls
//      `CAgentRelation::Teardown`.
//    - Save/Load (0x6F4A7ED0/0x6F4A7F10) - base Save/Load first, then
//      m_radius (WriteCFloat/ReadCFloat) and m_opt (WriteDwordAlt2/
//      ReadDwordAlt3), then the array's own header + every live element
//      via the new `WriteInterfLisEntries`/`ReadInterfLisEntries` leaves
//      (cpopointerflis_array.cpp) - Load's own leaf additionally
//      re-`CMemBlock::Init`s the array's own storage from the freshly-read
//      header (m_size/m_blockSize) before reading elements back into it,
//      the same "no separate append/Grow call - the freshly sized storage
//      IS the destination" shape confirmed directly off its own raw_asm.
//      Written naked, same register-caching-across-calls reasoning
//      cpopovelocitymod_saveload.cpp's own header already documents.
//    - slot 8 / NotifyEndpointAIfTagged (0x6F4A8870) - the short dispatcher
//      itself IS reconstructed this session (filters the incoming context
//      for the '^sig' tag, 0x5E736967, then reads/clears bit 1 of this
//      object's own +0x54/m_opt) - real C++, cpopointerflis_notify.cpp.
//      What it tail-jumps into, `sub_6F4A87F0`, is real now too (a later
//      follow-up session - see cpopointerflis_notifycore.cpp): ~40
//      instructions comparing m_radius against `g_CFloatZero`
//      (`dword_6FAAE470`) and, if this object's own +0x20 (base-chain
//      territory, unmodelled) is also clear, driving a further 6 callees
//      off `g_pTimeSync` (`dword_6FAB73D8`) - written as a naked
//      transcription (per pipeline/CLAUDE.md, a naked body should score
//      1.000 by construction) rather than a real C++ call sequence,
//      because none of its own six callees are reconstructed yet and a
//      wrong guess at any one of their signatures is exactly the
//      "compiles and looks plausible" trap this repo's own house rules
//      warn about - each is instead called by raw address through a
//      scratch register, the same `mov eax,<addr>/call eax` convention
//      Unit/unit_notifymodelchanged.cpp's own header already documents
//      for reaching an unreconstructed callee from inside a naked body.
//============================================================================
#ifndef CPOPOINTERFLIS_H
#define CPOPOINTERFLIS_H

#include "CFloat.h"
#include "cellbuffer.h"   // CMemBlock

struct SHandleRegistrationRequest;
class CDataStoreScratch;
class CDataStore;

namespace NIpse {

//----------------------------------------------------------------------------
//  One element of CPoPoInterfLis's own +0x58 array - three dwords, opaque
//  past "moved/stored/torn down as a unit" (see this file's own header
//  comment).
//----------------------------------------------------------------------------
struct SInterfLisEntry
{
    unsigned int m_field0;
    unsigned int m_field4;
    unsigned int m_field8;
};

//----------------------------------------------------------------------------
//  CPoPoInterfLis's own +0x58 array - same header shape as
//  Missile/bhpoprojectile.h's own `SBhPoTickRelaySlotArray` (a `CMemBlock`,
//  then m_growBy/m_capacity/m_count), holding 0x0C-byte `SInterfLisEntry`
//  records instead of raw 4-byte pointers.
//----------------------------------------------------------------------------
struct SInterfLisEntryArray
{
    //  0x6F4A7AB0 - remove `count` elements starting at `start`, clamped to
    //  what actually fits before m_count, sliding every element past the
    //  removed range down over the hole (`memmove`, 0x0C-byte stride) and
    //  shrinking m_count by however many were actually removed. `this` in
    //  ecx, `retn 8` - see Missile/bhpoprojectile_slots.cpp's own
    //  4-byte-stride `SBhPoTickRelaySlotArray::RemoveRange` for the
    //  identical shape this mirrors.
    int RemoveRange(unsigned int start, unsigned int count);

    CMemBlock    m_storage;   // +0x00
    unsigned int m_growBy;    // +0x14
    unsigned int m_capacity;  // +0x18
    unsigned int m_count;     // +0x1C
};

//  0x6F4A7B10 - write this array's own header (m_storage.m_blockSize,
//  m_storage.m_size, m_growBy, m_capacity, m_count, in that order) then
//  every live element's own three fields (WriteDword, WriteDword,
//  WriteDwordAlt2). `store` in ecx, `array` in edx, returns `store`.
CDataStoreScratch* __fastcall WriteInterfLisEntries(CDataStoreScratch* store,
                                                     SInterfLisEntryArray* array);

//  0x6F4A7BA0 - Save's mirror: reads the same five header fields back
//  directly into the array, re-`CMemBlock::Init`s its storage from the
//  freshly-read m_size/m_blockSize (header = blockSize - size), then reads
//  every element straight into that freshly sized storage (ReadDword,
//  ReadDword, ReadDwordAlt3) - no separate append/Grow call, since the
//  storage Init just established already has the right size.
CDataStore* __fastcall ReadInterfLisEntries(CDataStore* store,
                                             SInterfLisEntryArray* array);

class CPoPoInterfLis
{
public:
    //  0x6F48C7C0 - vtable slot 0. Literal string "PoPoInterfLis".
    const char* GetTypeName();

    //  0x6F48C910 - vtable slot 1. Same shape as every sibling in this
    //  chain (Agent/cporethresholdlis.h etc.), against this class's own
    //  static pool (CPoPoInterfLis_GetStaticPool,
    //  Agent/agentbasechain_staticpools.h).
    void ReturnToStaticPool();

    //  0x6F48C9A0 - vtable slot 2, scalar deleting destructor. Calls the
    //  real ~CPoPoInterfLis() first (below), frees `this` when bit 0 of
    //  `flags` is set (guarded by `this != 0`) - the
    //  `CRlAgent::ScalarDeletingDestructor`/`CPoPosBh::ScalarDeletingDestructor`
    //  shape, not the trivial restamp-only one this whole family otherwise
    //  uses.
    void* ScalarDeletingDestructor(unsigned int flags);

    //  0x6F48C950 - this class's own real destructor. Releases the +0x58
    //  array's own CMemBlock storage via the already-real
    //  `ReleaseCMemBlockBase` (Pathfinding/cellbuffer.h), then restamps to
    //  `??_7CPresence@NTempest@@6B@` - same restamp target every trivial
    //  destructor in this chain uses, reached through a real `~T()` here
    //  instead of directly (`CPoPosBh::~CPoPosBh`'s own precedent,
    //  Agent/poposbh.h).
    ~CPoPoInterfLis();

    //  0x6F4A79A0 - vtable slot 3. ResetEventBinding(arg) via the base,
    //  then m_radius from `arg`'s own +0x0C field (via the new leaf
    //  PoPoInterfLisQueryRadius, null-safe, g_CFloatZero default),
    //  m_opt = 0, then flags |= 0x190400.
    void Apply(SHandleRegistrationRequest* arg);

    //  0x6F4A7980 - Apply's own tail call, not a vtable slot. Stamps
    //  CAgentRelation::m_field44 (+0x44) from `*source` and takes a fresh
    //  channel registration timed off `&m_field44` itself
    //  (CAgentRelation::TakeRegistration, agentrelation.h) - the same
    //  "resample on Apply" tail a few siblings elsewhere in this wider
    //  family end their own Apply with. Not array-related.
    void SetField44AndSample(const void* source);

    //  0x6F4A8210 - vtable slot 4. See this file's own header comment.
    void Slot4(int arg);

    //  0x6F4A8870 - vtable slot 8. The short '^sig'-tag filter/bit-clear
    //  dispatcher itself, real. See this file's own header comment for why
    //  what it tail-jumps into (sub_6F4A87F0) is not.
    void* NotifyEndpointAIfTagged(const void* arg);

    //  0x6F4A87F0 - NotifyEndpointAIfTagged's own tail-jmp target, real
    //  now (see Agent/cpopointerflis_notifycore.cpp for the full trace).
    //  Not a vtable slot of its own - reached only via the tail-jmp above.
    void* NotifyEndpointACore(const void* arg);

    //  0x6F4A7ED0 / 0x6F4A7F10 - vtable slots 9/10. See this file's own
    //  header comment.
    void Save(void* writer);
    void Load(void* reader);

    //  0x6F48C7D0 - vtable slot 15. CAgentRelation::ComputeChecksum(hash)
    //  via the base, then folds m_radius then m_opt into `*hash`.
    void ComputeChecksum(unsigned int* hash);

    //  0x6F48C880 - vtable slot 16. CAgentRelation::DumpDebugState(sink)
    //  via the base, then "[PoLis]" (the class's own literal tag - not
    //  "[PoInterfLis]", confirmed byte-for-byte off aPolis) and
    //  "\trad[%s] opt[0x%X]" of m_radius/m_opt.
    void DumpDebugState(void* sink);

    //  ---- base-chain territory (CAgentRelation and below) - see
    //  Agent/agentrelation.h - plus this class's own added fields.
    char                  m_baseReserved00[0x50];
    CFloat                m_radius;   // +0x50
    unsigned int          m_opt;      // +0x54
    SInterfLisEntryArray  m_array;    // +0x58, 0x20 bytes
};

}  // namespace NIpse

#endif  // CPOPOINTERFLIS_H
