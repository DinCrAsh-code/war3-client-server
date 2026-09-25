//============================================================================
//  The 0x6F4A5xxx-0x6F4A6xxx "relation" object: a link between two agents
//  that observes both ends and relays events between them.
//
//  Named for the four-character code every event it sends carries in its
//  first two words - 0x5E72656C, which is `'^rel'` read most-significant
//  byte first, the same way CAbility's `'abil'` type tag is (see
//  docs/targets/trivial_accessor_sweep.md).  The individual event codes it
//  is asked to send are the same kind of tag: `'^end'` (0x5E656E64) for a
//  break, `'^ -l'` (0x5E202D6C) for an unlink.
//
//  The two endpoints are symmetric and everything about them is doubled:
//  each has a receiver pointer, a doubly-linked list node threading this
//  relation onto that endpoint's own list of relations, and its own bit in
//  the flags word deciding whether unlinking is announced or done silently.
//  Endpoint A is at +0x30, endpoint B at +0x40, and the shipped code has one
//  function per endpoint rather than one with an index - so this file does
//  too.
//
//  Only what this call tree reads is named.  The flags at +0x4C are a single
//  dword that is also read one byte at a time (`test [esi+4Eh], cl` is
//  bit 0x10000), so they are a union for the same reason
//  CGridRegistration::m_typeMask is.
//============================================================================
#ifndef AGENTRELATION_H
#define AGENTRELATION_H

#include "game.h"

//----------------------------------------------------------------------------
//  One node of an endpoint's relation list.  Embedded twice in
//  CAgentRelation, once per endpoint, and the unlink writes through the
//  neighbours' own copies - so `m_prev` really is at +0 and `m_next` at +4.
//----------------------------------------------------------------------------
struct SRelationLink
{
    //  0x6F4A5BE0 - splice `node` into this list immediately after this
    //  node.  `this` is the list head an endpoint hands out (the endpoint's
    //  own embedded link), `node` is the relation's own m_linkA/m_linkB.
    void InsertAfter(SRelationLink* node);

    SRelationLink* m_prev;      // +0x00
    SRelationLink* m_next;      // +0x04
};

//----------------------------------------------------------------------------
//  What a relation's endpoint receives.  The event goes out through the
//  endpoint's own vtable slot 0x20, carrying the same 11-dword context
//  CAgentTickRelay::FireEvent builds (agenttickrelay.h) - which is why the
//  layout below is that struct's and not a new one.
//----------------------------------------------------------------------------
struct SAgentTickEventCtx;
struct SHandleRegistrationRequest;

class CAgentRelation
{
public:
    //  0x6F47FFD0 - NIpse::CPrRelation's own vtable slot 0. `retn` with zero
    //  stack bytes, no `this` read - the literal string "PrRelation", the
    //  same shape Pathfinding/crlagent_gettypename.cpp's own
    //  `CRlAgent_GetTypeName` and Agent/presence.h's own
    //  `CPresence::GetTypeName`/`CPresenceTagged::GetTypeName` already
    //  establish one level down and one level up this chain.
    const char* GetTypeName();

    //  0x6F480060 - NIpse::CPrRelation's own vtable slot 1. Same
    //  unlink-then-return-to-pool shape as Agent/presence.h's own
    //  CPresence::ReturnToStaticPool/CPresenceTagged::ReturnToStaticPool,
    //  against this class's own static pool
    //  (CPrRelation_GetStaticPool, Agent/agentbasechain_staticpools.h).
    void ReturnToStaticPool();

    //  0x6F47FFF0 - NIpse::CPrRelation's own vtable slot 2, scalar deleting
    //  destructor.  Byte-identical (per agent_worktrees/funcs/'s own
    //  raw_bytes) to CPresence's and CPresenceTagged's own scalar deleting
    //  destructors apart from address - see Agent/presence.h's own header
    //  comment for the full oddity this trio shares: no base teardown call,
    //  no `this != 0` guard, restamps straight to
    //  `??_7CPresence@NTempest@@6B@` rather than this class's own vtable.
    void* ScalarDeletingDestructor(unsigned int flags);

    //  0x6F4A5F40 / 0x6F4A5FA0 - tell one endpoint something.  Identical
    //  bodies over the two endpoint slots, except that B's context carries a
    //  1 where A's carries a 0 in the field after the subject.
    void PostToEndpointA(unsigned int code, void* detail, void* extra);
    void PostToEndpointB(unsigned int code, void* detail, void* extra);

    //  0x6F4A6010 - and the same thing to *itself*, through its own vtable
    //  slot 0x20.  Its context names no subject at all.
    void PostToSelf(unsigned int code, void* detail);

    //  0x6F4A6750 - take a channel registration for this relation, if it
    //  wants one and does not have one yet.  Which endpoints have to be
    //  present for that is decided by three bits of the flags word.
    void AcquireChannel();

    //  0x6F4A67D0 / 0x6F4A6830 - drop one endpoint: unlink this relation
    //  from that endpoint's list (announcing it first when the endpoint's
    //  own "announce" bit is set), clear the endpoint, break the relation
    //  and re-decide the channel.
    void DetachEndpointA();
    void DetachEndpointB();

    //  0x6F4A6AF0 - the mirror image of Break(): both endpoints are now
    //  present, so the relation is live.  Mark it (0x200000, the counterpart
    //  of Break's 0x400000), take the channel registration if the flags ask
    //  for one, announce '^beg' to whichever of the three parties are
    //  subscribed, and clear the mark again.
    void Establish();

    //  0x6F4A6B70 - point endpoint A at `endpoint` and thread this relation
    //  onto `list`, the endpoint's own relation list.  Whatever endpoint A
    //  was is dropped first.  The 0x1000 bit picks announcing the link to
    //  the new endpoint over splicing the node in silently - the same
    //  announce-or-splice pair DetachEndpointA has for the other direction.
    void AttachEndpointA(void* endpoint, SRelationLink* list);

    //  0x6F4A64A0 - the relation is over: mark it, tell whichever of the
    //  three parties are subscribed, and release the channel registration.
    //  Re-entrant by construction - the 0x400000 bit it sets is the guard
    //  its own first test reads - and it clears that bit again on the way
    //  out.
    void Break();

    //  0x6F4A68E0 / 0x6F4A6890 - release and (re-)take the channel
    //  registration.  Releasing also drops whichever endpoints the flags say
    //  the registration was holding.
    void ReleaseRegistration();
    void TakeRegistration(const CFloat* value, int time);

    //  0x6F4A6BC0 - the mirror image of AttachEndpointA for endpoint B: point
    //  endpoint B at `endpoint` and thread this relation onto `list`.  The
    //  announce-or-splice choice is bit 0x2000 here rather than
    //  AttachEndpointA's 0x1000 - see agentrelationattachendpointb.cpp.
    void AttachEndpointB(void* endpoint, SRelationLink* list);

    //  0x6F4A6920 - full teardown: detach both endpoints, retire both
    //  channel registrations (the 0x10000 "retired" bit game.h's own
    //  SSyncRegistration comment documents) and unregister this relation's
    //  handle-table entry.  See agentrelationteardown.cpp.  `arg` is passed
    //  straight through to SHandleTableEntry::UnregisterAndNotify, which
    //  never reads its own argument either - kept for the same reason
    //  DetachRelation's own unread `alsoEndpointB`-adjacent argument is.
    void Teardown(int arg);

    //  0x6F4A69B0 - CAgentBaseAbs's own vtable slot 18 (+0x48), reached
    //  through this class's own layout (this call tree never establishes
    //  a CAgentBaseAbs override at this slot - it just operates on
    //  CAgentRelation's fields and methods, so it reads as CPrRelation's
    //  own inherited virtual rather than something CAgentBaseAbs adds).
    //  `registration` is compared against this relation's own m_channelReg
    //  (+0x20): if it *is* this relation's channel registration, the
    //  notification is redirected back through this object's own vtable
    //  slot 4 (CAgentBaseAbs's `+0x10`, a hand-written indirect dispatch -
    //  see agentrelationteardown.cpp) with argument 0; otherwise it
    //  announces '^sig' (0x5E736967) to whichever of self/A/B the flags
    //  word's 0x400/0x100/0x200 bits ask for, then - if `registration` is
    //  this relation's OWN m_registration (+0x1C) and is not already
    //  retired - releases the registration.  The 0x800000 flag bit set for
    //  the whole body and cleared on every exit reads as a reentrancy
    //  guard, the same shape Break's 0x400000 and Establish's 0x200000 are.
    void OnRegistrationRetired(void* registration);

    //  0x6F4A6570 / 0x6F4A6C10 - this class's own save/load pair, called
    //  from `NIpse::CRlAgent::Save` (0x6F4A4EE0, +0x24, Pathfinding/
    //  crlagent.h, agentbaseabssave.cpp - reached through, but not owned
    //  by, CAgentBaseAbs's own vtable slot 9) and its `NIpse::CRlAgent::Load`
    //  counterpart's own call (0x6F4A5AD0, +0x28, agentbaseabsload.cpp).  Writes/
    //  reads, in order: m_field44, m_eventId, m_flags, the two endpoint
    //  link+owner+endpoint quads (m_linkA's and m_linkB's own extended
    //  0x10-byte shape - see agentrelationsyncsave.cpp's own header
    //  comment for why SRelationLink's documented 8 bytes turn out to be
    //  half of a real 0x10-byte per-endpoint block), then m_registration
    //  and m_channelReg.  Load additionally re-derives this relation's own
    //  sync channel (CTimeSync::GetChannel, off g_pTimeSync) and re-issues
    //  both registrations through it rather than blitting the raw fields
    //  back - a deserialized SSyncRegistration has to go back through the
    //  channel's own bookkeeping (MakeRequest), not be recreated by value.
    //  See agentrelationsyncsave.cpp / agentrelationsyncload.cpp.
    void Save(void* writer);
    void Load(void* reader);

    //  0x6F4A60A0 - CPrRelation's own vtable slot 15 (+0x3C), confirmed by
    //  the CPresence -> ... -> CAgentBaseAbs slot-target table this session
    //  built from every class's own `agent_worktrees/classes/<ADDR>.json`:
    //  CPresenceTagged overrides slot 15 first
    //  (`NTempest::CPresenceTagged::FoldLinkFieldsIntoHash`, 0x6F4AEE60,
    //  Agent/presencetagged.h), and this class overrides it a second time
    //  with this address - CRlAgent and CAgentBaseAbs never touch the slot
    //  again, so a live CAgentBaseAbs object's vtable slot 0x3C really does
    //  read this address, but the class that actually installs it there is
    //  `NIpse::CPrRelation`/`CAgentRelation`, not `CAgentBaseAbs` (this was
    //  previously misfiled as `CAgentBaseAbsRaw::ComputeChecksum`). See
    //  Agent/agentbaseabscomputechecksum.cpp for the fold body and the full
    //  checksum-position investigation, unaffected by this rename - same
    //  address, same behaviour, only the owning class changed.
    void ComputeChecksum(unsigned int* hash);

    //  0x6F4A6070 - see agentbaseabseventbinding.cpp.  NOTE: reached from
    //  `NIpse::CRlAgent::Apply` (0x6F4A4D70, Pathfinding/crlagent.h,
    //  Agent/agentbaseabs_slot3.cpp - reached through, but not owned by,
    //  CAgentBaseAbs's own vtable slot 3; a *different* parallel session's
    //  own batch of this same dump) through a forward declaration of its own
    //  (`CAgentBaseAbsSlot3Externals::SetSomething_6F4A6070(void*)`) rather
    //  than this one - an expected cross-batch naming seam CLAUDE.md's own
    //  task assignment flagged, for the orchestrator's integration pass to
    //  reconcile once both batches land (link_check.py will find it).
    void ResetEventBinding(SHandleRegistrationRequest* request);

    //  0x6F4A62D0 - dump this relation's own state (presence/birth tag,
    //  m_field44/m_eventId/m_flags formatted, then endpoint A's own
    //  presence/birth tag pair printed twice, once labelled "alph" and once
    //  "beta" - see agentbaseabsdebugdump.cpp's own header comment for why
    //  that duplication is real, not a transcription slip) into a debug
    //  text sink (Storm/textsink.h).  Reached from CAgentBaseAbs's own
    //  vtable slot 16 (+0x40, 0x6F47FE50,
    //  Agent/cagentbaseabs_module_batch2.cpp - another parallel session's
    //  batch) as a side effect ahead of forwarding to the delegate's own
    //  DumpState.
    void DumpDebugState(void* sink);

    //  +0x00-+0x1C: base-class territory (CPresenceTagged/CPresence/
    //  CLinkedNode<CPresence>), not this class's own - confirmed by two
    //  independent inlined-base-constructor call sites this session found
    //  (sub_6F4801E0/CRlAgent::CRlAgent, sub_6F487390/CRlProperty::CRlProperty:
    //  both zero this range - and +0x14/+0x18 to 0xFFFFFFFF specifically -
    //  before either one's own `mov [eax], offset ??_7CPrRelation@NIpse@@6B@`
    //  stamp).  Those same two call sites independently confirm this class's
    //  own construction runs from there through +0x4F (see the fields below),
    //  matching this header's existing layout exactly.
    char            m_reserved00[0x1C];
    //  +0x1C / +0x20 - two channel registrations.  Whatever they point at
    //  carries a flags dword at +0x10: 0x10000 marks a registration this
    //  relation has finished with, bit 0 one it wants sampled.
    SSyncRegistration* m_registration;      // +0x1C
    SSyncRegistration* m_channelReg;        // +0x20
    SRelationLink   m_linkA;                // +0x24 / +0x28
    char            m_reserved2C[0x30 - 0x2C];
    void*           m_endpointA;            // +0x30
    SRelationLink   m_linkB;                // +0x34 / +0x38
    char            m_reserved3C[0x40 - 0x3C];
    void*           m_endpointB;            // +0x40
    //  +0x44 - initialised from a fixed global (`dword_6FAAE470`, the same
    //  value at both inlined-base-ctor sites above) rather than zero or a
    //  per-instance value - a build-time constant, not a coordinate.
    //  UPDATE (agentrelationsyncsave.cpp/agentrelationsyncload.cpp): this
    //  call tree *does* read and write it now, as a plain `CFloat` -
    //  Save/Load round-trip it through WriteCFloat/ReadCFloat right before
    //  m_eventId/m_flags. Named `m_field44` rather than anything more
    //  specific: nothing in either call tree establishes what it actually
    //  represents beyond "a CFloat", so this still doesn't settle the
    //  checksum investigation's own open "is this a position" question -
    //  see docs/notes/checksum-provider-registry.md. Recorded here because
    //  it, +0x48 and +0x4C are exactly the
    //  three consecutive dwords this class's own `ComputeChecksum`
    //  (Agent/agentbaseabscomputechecksum.cpp, `sub_6F4A60A0`, declared
    //  above in this class - not `CAgentBaseAbs`'s, see this rename's own
    //  vtable-derived evidence there -
    //  `TempestProviderListHash`'s per-entry vtable-slot-0x3C desync-hash
    //  callback) folds unconditionally - see that file's own header and
    //  docs/notes/checksum-provider-registry.md's "checksum-position-
    //  investigation" section for the callback itself. This class's own
    //  constructor evidence settles that section's still-open "is this a
    //  position" question for these three fields specifically: **no** -
    //  a fixed build constant, an event-id sentinel and a flags word, not
    //  a coordinate triple. (Whether the *conditional* fold of
    //  `m_endpointA`'s own +0x14/+0x18 reaches something position-derived
    //  is untouched by this and remains open.)
    CFloat          m_field44;               // +0x44 - see the comment above
    //  +0x48 - the message id whoever bound this relation wants back when
    //  it fires.  FloatListener::Bind (floatlistenerbind.cpp) stamps it on
    //  the relation the agent maker has just built, immediately before
    //  taking the channel registration.  Initialised to 0xFFFFFFFF (a
    //  "no event bound yet" sentinel) by the inlined base ctor - see above.
    unsigned int    m_eventId;              // +0x48
    union
    {
        unsigned int  m_flags;              // +0x4C
        struct
        {
            unsigned char m_flagBytes[2];
            unsigned char m_flagByte4E;     // +0x4E, bit 0 == m_flags 0x10000
        } m_bits;
    };
};

//----------------------------------------------------------------------------
//  0x6F4A4B40 - drop a relation's endpoints, but only the ones it still has
//  a list node for.  A free `__stdcall` function rather than a member: both
//  arguments arrive on the stack and it cleans eight bytes, and it never
//  touches ecx.  `alsoEndpointB` is what its two callers differ in -
//  CAgent::SetTrackedTarget passes 0 and detaches endpoint A only.
//----------------------------------------------------------------------------
void __stdcall DetachRelation(CAgentRelation* relation, int alsoEndpointB);

//  0x6F4A4C70 - detach `relation`'s endpoint B, but only if it is actually
//  threaded onto that endpoint's list (either link word non-null) - the
//  same "still linked?" gate DetachRelation's own endpoint-B half already
//  has, pulled out as its own free function because at least one caller in
//  this dump (Agent/agentbaseabs_relatedaccessors.cpp's per-index detach
//  accessor) reaches it directly rather than through DetachRelation.
void __stdcall DetachEndpointBIfLinked(CAgentRelation* relation);

//  Flag bits this call tree reads, by what reads them.
const unsigned int kRelBreakTellSelf     = 0x00000010;   // Break -> PostToSelf
const unsigned int kRelBreakTellA        = 0x00000020;   // Break -> endpoint A
const unsigned int kRelBreakTellB        = 0x00000040;   // Break -> endpoint B
const unsigned int kRelAnnounceUnlinkA   = 0x00004000;
const unsigned int kRelAnnounceUnlinkB   = 0x00008000;
const unsigned int kRelWantSample        = 0x00010000;
const unsigned int kRelDropBOnRelease    = 0x00020000;
const unsigned int kRelDropAOnRelease    = 0x00040000;
const unsigned int kRelBreakDropsChannel = 0x00100000;
const unsigned int kRelEstablishing      = 0x00200000;
const unsigned int kRelBreaking          = 0x00400000;

//  Establish's own four bits.  The three "tell" bits are the low three of
//  the same word, tested one byte at a time exactly as Break's 0x10/0x20/
//  0x40 trio is; the registration bit is tested as a full dword.
const unsigned int kRelBeginTellA        = 0x00000001;
const unsigned int kRelBeginTellB        = 0x00000002;
const unsigned int kRelBeginTellSelf     = 0x00000004;
const unsigned int kRelEstablishTakesReg = 0x00080000;

//  AttachEndpointA's announce-or-splice bit.
const unsigned int kRelAnnounceLinkA     = 0x00001000;
const unsigned int kRelNeedsBoth         = 0x10000000;
const unsigned int kRelNeedsA            = 0x20000000;
const unsigned int kRelNeedsB            = 0x40000000;
const unsigned int kRelNeedsAny          = 0x70000000;

//----------------------------------------------------------------------------
//  0x6F4A9xxx - a relation that drives one value towards a target.
//
//  It *is* a CAgentRelation: 0x6F4A9B70 calls 0x6F4A68E0 and 0x6F4A6890 -
//  ReleaseRegistration and TakeRegistration - on its own `this`, so the base
//  class's whole layout is underneath these two fields.  Its endpoint B is
//  the CPathTrace being driven, which is what the one-instruction getter at
//  0x6F4A9A50 (`mov eax,[ecx+40h]`) hands back.
//
//  Only the two fields this call tree touches are named; whatever else the
//  class carries past +0x58 is not recoverable from here.
//----------------------------------------------------------------------------
class CDrivenRelation : public CAgentRelation
{
public:
    //  0x6F4A98F0 - is `value` close enough to the target to stop?  The
    //  tolerance is a global, not a member.
    int AtTarget(const CFloat* value);

    //  0x6F4A9C70 - the per-update entry point: at the target, drop the
    //  registration and announce arrival to endpoint A; short of it,
    //  recompute.
    void Update(const CFloat* value);

    //  0x6F4A9B70 - recompute the drive from the trace's own ramp: drop the
    //  current channel registration and, if the ramp is still moving the way
    //  m_mode asks, take a fresh one for the arrival.  See
    //  agentrelationrecompute.cpp.
    void Recompute();

    //  0x6F4A9A50 - endpoint B, typed.  One instruction (`mov eax,[ecx+40h]`),
    //  and the CPathTrace conclusion is Resample()'s own two calls on the
    //  result - GetTravelDistance and RampValueAt, the pair game.h says is
    //  only ever asked of a trace.  It used to be the placeholder
    //  `FieldHolder_6F4A9A50::Get` in src/Misc/misc_field_getters.cpp; the
    //  header of that file says a placeholder goes when the class is known.
    CPathTrace* GetTrace();

    //  0x6F4A9E20 - re-read the driven value out of the trace and put it
    //  through Update(); with no trace, or one that has retired itself,
    //  drop the channel registration instead.  See
    //  agentrelationresample.cpp.
    void Resample();

    //  0x6F4A9FF0 / 0x6F4AA010 - publish a new target or a new mode, and
    //  Resample() when the caller asks for it.  The `resample` flag is what
    //  lets FloatListener::SetBoundary set both and sample once.
    void SetTarget(const CFloat* target, int resample);
    void SetMode(int mode, int resample);

    CFloat m_target;    // +0x50
    int    m_mode;      // +0x54
};

//  flt_6FAB7778 - how close to the target counts as arrived.
extern const float g_driveTolerance;

#endif  // AGENTRELATION_H
