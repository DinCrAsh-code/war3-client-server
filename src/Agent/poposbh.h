//============================================================================
//  NIpse::CPoPosBh (`??_7CPoPosBh@NIpse@@6B@`, 0x6F952EF4, 24 slots) - one
//  of CPoPos's (popos.h) two sibling extensions (the other is CPoPosCl,
//  poposcl.h), part of the same "PoPos*" cluster-H follow-up.
//
//  Slots 5-8, 11-14 and 17 are the shared no-ops (see popos.h's own header
//  comment). Slot 18 - UNLIKE CPoPos/CPoPosCl, which inherit the shared
//  `CAgentRelation::OnRegistrationRetired` unchanged - has its OWN
//  override here (0x6F4AB460): if the incoming handle matches this
//  object's own +0xBC registration, call its own Slot4-shaped teardown
//  (0x6F4AB250) directly; otherwise tail-jump into the shared
//  `CAgentRelation::OnRegistrationRetired` (0x6F4A69B0) for the general
//  case. Not reconstructed this session (0x6F4AB250's own callee
//  0x6F4AACC0 is a several-dozen-instruction CFloat-threshold routine this
//  session did not confidently re-derive) - left `TODO`, same caution as
//  Apply's own gap in popos.h.
//
//  FLAT, offset-addressed, exactly like popos.h's own CPoPos (NOT
//  `: public CPoPos` - see that file's header comment for why real
//  inheritance from a class that itself has no complete modelled size is
//  unsafe here; CPoPosBh's own added fields start at the shipped +0xC0,
//  not immediately after CPoPos's own last modelled field at +0x98).
//  Every base-class-shaped call below reaches `NIpse::CPoPos`'s own
//  methods via the same `((CPoPos*)this)->Method(...)` reinterpret-cast
//  convention CPoPos itself already uses to reach `CRlProperty`.
//
//  **Update (further follow-up session): the +0x9C..+0xBF gap is real
//  now, not a placeholder.** Tracing `Slot4` (0x6F4AB430) and `Save`/`Load`
//  (0x6F4AAEE0/0x6F4AB480) end to end settled it:
//
//    +0x9C  m_relatedAgents - a `SBhPoTickRelaySlotArray` (0x20 bytes,
//           Missile/bhpoprojectile.h) EMBEDDED here, byte-for-byte the same
//           growable-pointer-array struct `CBhPoProjectile` keeps at its
//           own +0x9C for its tick-relay slots - confirmed by `Slot4`'s own
//           teardown call reaching `SBhPoTickRelaySlotArray::Grow`
//           (0x6F4AB050) with the array based at `this+0x9C`, and by `Load`
//           calling that exact same address the exact same way. This is
//           shared generic array machinery, not a coincidence of two
//           classes independently choosing the same layout - see
//           `Agent/poposbh_relatedagents.cpp`'s own header comment.  Old
//           +0xA8/+0xB8 field names (`m_relatedAgents`/count) now fold into
//           this struct's own `m_storage.m_data`/`m_count`.
//    +0xBC  m_syncReg - `SSyncRegistration*` (game.h) - the SAME
//           registration-record type `CBhPoProjectile::m_pendingRegistration`
//           holds (bhpoprojectile.h), confirmed by `Save`'s own tail
//           streaming exactly `SSyncRegistration`'s own
//           m_deadline/m_value/m_sequence/m_owner-time/m_flags&1 fields (in
//           that order) and `Load`'s own mirror re-deriving it through
//           `CSyncChannel::MakeRequest` (game.h) - NOT the
//           `CBhPoProjectile::ResampleSyncChannel` target/source CFloat
//           this comment used to (correctly, separately) name at +0xC0
//           below; that CFloat and this registration pointer are two
//           different fields four bytes apart.
//
//  CPoPosBh's own added fields (all nine confirmed CFloat-shaped by
//  DumpDebugState's own three dump lines - "pdl[%s] for[%s] rot[%s]",
//  "acc[%s] ang[%s] lvl[%s] ldl[%s]", "brk[%s] prw[%s]"):
//
//    +0xC0  m_pdl   +0xC4  m_forField   +0xC8  m_rot
//    +0xCC  m_acc   +0xD0  m_ang        +0xD4  m_lvl   +0xD8  m_ldl
//    +0xDC  m_brk   +0xE0  m_prw
//============================================================================
#ifndef POPOSBH_H
#define POPOSBH_H

#include "popos.h"
#include "bhpoprojectile.h"   // SBhPoTickRelaySlotArray - see the +0x9C field below
#include "game.h"             // SSyncRegistration - see the +0xBC field below

namespace NIpse {

class CPoPosBh
{
public:
    //  0x6F48CAE0 - vtable slot 0. Literal "PoPosBh".
    const char* GetTypeName();

    //  0x6F48CF00 - vtable slot 1. Same shape as every sibling in this
    //  chain, against this class's own static pool
    //  (CPoPosBh_GetStaticPool, agentbasechain_staticpools.h).
    void ReturnToStaticPool();

    //  0x6F48CFA0 - vtable slot 2, scalar deleting destructor. UNLIKE
    //  CPoPos's own trivial restamp, this one calls a REAL destructor
    //  first (0x6F48CF40 below) and carries the `this != 0` guard on the
    //  free - the shape Pathfinding/crlagent.h's own
    //  `CRlAgent::ScalarDeletingDestructor` documents for whenever a real
    //  destructor call precedes the free.
    void* ScalarDeletingDestructor(unsigned int flags);

    //  0x6F48CF40 - CPoPosBh's own real destructor. Destructs the +0x9C
    //  `NTempest::CMemBlock`-shaped sub-object via `ReleaseCMemBlockBase`
    //  (already real, Agent/cmemblock.h), then restamps to
    //  `??_7CPresence@NTempest@@6B@` - same restamp target every trivial
    //  destructor in this chain uses, just reached through a real `~T()`
    //  here instead of directly.
    ~CPoPosBh();

    //  0x6F4AB390 - vtable slot 3, "Apply". Base `CPoPos::Apply` first,
    //  then stamps +0xC0 with a fixed constant, resolves +0xC4/+0xC8 from
    //  `arg` (or shared defaults) the same "resolve or default" shape
    //  popos.h's own Apply doc comment describes, zeroes +0xCC through
    //  +0xE0, then calls `CBhPoProjectile::ResampleSyncChannel` (already
    //  real, 0x6F4AB1A0) against the +0xC0 CFloat.
    void Apply(SHandleRegistrationRequest* arg);

    //  0x6F4AB430 - vtable slot 4, "Slot4". Sets the 0x1000000 flags bit,
    //  tears down every live slot of the +0x9C related-agent array AND the
    //  array's own storage (`TeardownRelatedAgents(notify=1, shrink=0)`,
    //  0x6F4AB2E0, Agent/poposbh_relatedagents.cpp), clears this object's
    //  own +0xBC sync registration (0x6F4AB130 below), then tail-calls the
    //  base `CPoPos::Slot4`.  Resolved this follow-up session - see
    //  poposbh_relatedagents.cpp's own header comment for the full trace
    //  (the "array shrink/rebalance helpers" this session's own earlier
    //  pass could not confidently sign).
    void Slot4(int arg);

    //  0x6F4AAEE0 - vtable slot 9, Save. Base `CPoPos::Save` first, then
    //  this class's own nine added CFloats (+0xC0 through +0xE0, in
    //  offset order), the +0x9C related-agent array's own count and every
    //  live entry (`WritePathRefEntry` per slot), then a "have a sync
    //  registration" flag and - if set - every one of that
    //  `SSyncRegistration`'s own fields.  See poposbh_relatedagents.cpp's
    //  header comment - resolved this follow-up session.
    void Save(CDataStoreScratch* store);

    //  0x6F4AB480 - vtable slot 10, Load. Save's near-mirror: reads the
    //  array back one `SPathRefReader::ReadEntry()` at a time, appending
    //  each through the SAME `SBhPoTickRelaySlotArray::Grow` `Slot4`'s own
    //  teardown calls (0x6F4AB050 - shared code with `CBhPoProjectile`,
    //  bhpoprojectile.h), then re-derives (rather than streams verbatim)
    //  which of `CTimeSync`'s own two embedded sync channels (+0x14/+0x68 -
    //  `SelectSyncChannel`, 0x6F483270) to register the +0xBC
    //  `SSyncRegistration` on via `CSyncChannel::MakeRequest`, the one
    //  real asymmetry with Save.  See poposbh_relatedagents.cpp's header
    //  comment - resolved this follow-up session.
    void Load(CDataStore* store);

    //  0x6F48CAF0 - vtable slot 15, ComputeChecksum. Folds the base
    //  `CPoPos::ComputeChecksum` first, then this class's own nine added
    //  CFloats (+0xC0 through +0xE0 inclusive, in offset order).
    void ComputeChecksum(unsigned int* hash);

    //  0x6F48CDB0 - vtable slot 16, DumpDebugState. Dumps the base
    //  `CPoPos::DumpDebugState` first, then this object's own "[PosBh]"
    //  tag and its own three field lines.
    void DumpDebugState(void* sink);

    //  0x6F4AB460 - vtable slot 18, own override. Real now
    //  (Agent/poposbh_slot18.cpp): if the incoming handle (the one stack
    //  argument) matches this object's own +0xBC registration, call
    //  Slot18Helper() directly; otherwise tail-jump into the shared
    //  `CAgentRelation::OnRegistrationRetired` (0x6F4A69B0) for the
    //  general case.
    void Slot18(void* handle);

    //  0x6F4AB250 - Slot18's own helper: clear +0xBC via
    //  `ClearBcRegistration` (0x6F4AB130), then - only if `Slot18Gate()`
    //  returns non-zero - resample via the already-real
    //  `CBhPoProjectile::ResampleSyncChannel` (0x6F4AB1A0) against +0xC0
    //  (m_pdl). Agent/poposbh_slot18.cpp.
    void Slot18Helper();

    //  0x6F4AACC0 - the gate `Slot18Helper` calls. Real now
    //  (Agent/poposbh_slot18_gate.cpp - a follow-up session's own naked
    //  transcription; see that file's own header for the full trace and
    //  why it stays naked). Not a vtable slot of its own.
    int Slot18Gate();

    //  0x6F4AB130 - clear +0xBC's own sync registration's "sample me" bit
    //  and this object's own pointer to it, with no teardown call of its
    //  own (unlike TeardownRelatedAgents, which calls a vtable slot on
    //  each array entry) - the registration itself is released elsewhere.
    //  Shared by `Slot4` above and idx18's own still-`TODO` helper.
    void ClearBcRegistration();

    //  0x6F4AB2E0 - the array-teardown `Slot4` calls.  See
    //  poposbh_relatedagents.cpp for the full trace.
    void TeardownRelatedAgents(int notify, int shrink);

    char   m_base00_9C[0x9C];   // +0x00-+0x9B, CPoPos's own territory
                                  // (unmodeled here - see this file's own
                                  // header comment).
    SBhPoTickRelaySlotArray m_relatedAgents;   // +0x9C, 0x20 bytes
    SSyncRegistration*      m_syncReg;         // +0xBC
    CFloat m_pdl;        // +0xC0
    CFloat m_forField;   // +0xC4
    CFloat m_rot;        // +0xC8
    CFloat m_acc;        // +0xCC
    CFloat m_ang;        // +0xD0
    CFloat m_lvl;        // +0xD4
    CFloat m_ldl;        // +0xD8
    CFloat m_brk;        // +0xDC
    CFloat m_prw;        // +0xE0
};

//  0x6F4AAB70 - Apply's own "resolve or default" helper: `*out = req ?
//  *(const unsigned int*)((const char*)req + 0x10) : g_unk6FAAE624`,
//  hidden-pointer-return convention (same shape popos.h's own
//  0x6F4A71F0 would have had, had CPoPos::Apply not been left a THUNK -
//  UNLIKE that one, this class's own Apply has no ambiguous tail, so both
//  of its own helpers are fully reconstructed).
unsigned int* __fastcall PickRegistrationFloatOrDefault_0x10(
    unsigned int* out, const SHandleRegistrationRequest* req);

//  0x6F4AABB0 - same shape, `req + 0x14`.
unsigned int* __fastcall PickRegistrationFloatOrDefault_0x14(
    unsigned int* out, const SHandleRegistrationRequest* req);

}  // namespace NIpse

#endif  // POPOSBH_H
