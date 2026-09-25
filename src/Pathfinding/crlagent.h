//============================================================================
//  NIpse::CRlAgent (`??_7CRlAgent@NIpse@@6B@`, 0x6F95207C) - part of the
//  single-inheritance RTTI chain
//
//      CAgentBaseAbs : public NIpse::CRlAgent : public NIpse::CPrRelation :
//        public NTempest::CPresenceTagged : public NTempest::CPresence :
//        public NTempest::CLinkedNode<NTempest::CPresence>  (mdisp:4)
//
//  investigated for `TempestProviderListHash` (Net/tempestproviderlisthash.cpp,
//  0x6F543D40) - see docs/notes/checksum-provider-registry.md - which folds a
//  live registrant's vtable-slot-0x3C callback into the network desync
//  checksum, and a live debugger session confirming one registered object
//  carries `CAgentBaseAbs::vftable`.
//
//  CORRECTION (2026-09-07, vtable-derived - see the chain's own slot-target
//  table this session built from every class's `agent_worktrees/classes/
//  <ADDR>.json`): slot 0x3C (index 15) is **not** CAgentBaseAbs's own slot,
//  and `sub_6F4A60A0` is not the function actually installed there. Walking
//  the chain, CPresenceTagged's own slot 15 is `0x6F4AEE60`
//  (`NTempest::CPresenceTagged::FoldLinkFieldsIntoHash`), CPrRelation
//  overrides it with `0x6F4A60A0` (`CAgentRelation::ComputeChecksum` -
//  `CAgentRelation` being the established alias for `NIpse::CPrRelation`,
//  agentrelation.h), and neither CRlAgent nor CAgentBaseAbs ever touches
//  the slot again - so a live object's `vftable[0x3C]` really does read
//  `0x6F4A60A0` (same address, same behaviour, nothing about the checksum
//  investigation's own conclusion about *what gets hashed* changes), but
//  the class whose vtable actually first installs that value at that slot
//  is `NIpse::CPrRelation`/`CAgentRelation`, not `CAgentBaseAbs`. See
//  Agent/agentbaseabscomputechecksum.cpp's own updated header for the
//  full reasoning, in the same "correction, not deletion" style as this
//  file's own NotifyField2Group correction pattern
//  (Agent/agentbaseabsrelatedcascade.cpp).
//
//  Every base class below CRlAgent (CPrRelation, CPresenceTagged, CPresence,
//  CLinkedNode<CPresence>) now has its own `agent_worktrees/classes/<ADDR>.json`
//  entry too (the operator's 2026-09-07 re-dump, after the namespace/
//  template regex fix - see CLAUDE.md's own "Caveat for classes/" note), but
//  none of those classes has a real C++ layout reconstructed in this repo
//  yet, so this remains a flat, offset-addressed layout rather than a real
//  C++ inheritance chain: the ctor/dtor below reproduce the *exact*
//  byte-for-byte writes the shipped instruction stream makes (CLAUDE.md's
//  "Struct offsets are exact, and only touched members get names"), without
//  depending on those classes' own still-unmodeled headers.  A later
//  integration pass can retarget the `m_base*` fields onto the real base
//  classes once they exist here.
//
//  ---------------------------------------------------------------------
//  What CRlAgent's OWN constructor actually initializes (the deliverable
//  this reconstruction was asked to nail down):
//
//  sub_6F4801E0 (CRlAgent::CRlAgent) writes offsets 0x04 through 0x4C
//  *before* it stamps CRlAgent's own vtable pointer at offset 0 (0x480262).
//  Every one of those writes - the four dwords at +0x04.. +0x10, the two
//  -1 sentinels at +0x14/+0x18, the zeroed run at +0x1C..+0x40, the two
//  self-pointers at +0x2C/+0x3C (a CLinkedNode-shaped empty-list head/tail),
//  and, most importantly, +0x44 = &g_CFloatZero / +0x48 = -1 / +0x4C = 0 -
//  therefore belongs to the INLINED BASE CHAIN (CPresence/CPresenceTagged/
//  CPrRelation - the compiler folded their trivial constructors into this
//  same function, stamping only CPrRelation's own vtable at +0 along the
//  way, at 0x480219), not to CRlAgent itself.
//
//  CRlAgent's *own* contribution starts only *after* the 0x480262 vtable
//  restamp: offsets 0x50 and 0x54 (both zeroed), and the CDynTable<CPrRelation*>
//  sub-object at 0x58 (0x20 bytes, a capacity-8 table of related agents -
//  see crlagent_ctor.cpp), followed by a zeroed tail from 0x78 through 0xB4
//  inclusive (0x40 bytes). Total object size established by this
//  constructor: 0xB8 bytes.
//
//  **This directly narrows the checksum investigation**: this+0x44 is
//  provably NOT CRlAgent's own field, and at construction time it copies
//  `g_CFloatZero`'s own bit pattern (Math/CFloat.h's shared zero-float
//  constant - the shipped code loads the 4 bytes stored at
//  `dword_6FAAE470`, i.e. dereferences the global, rather than embedding an
//  immediate 0; the field is a `CFloat`, not a raw int, which is why the
//  source routes through the named constant instead of a literal) - so at
//  construction its value is a plain `0x00000000`, exactly as inert as an
//  immediate zero would have been - with +0x48/+0x4C a -1 sentinel and a
//  zero flag next to it. If nothing later overwrites +0x44 with a real
//  per-agent `CFloat`, sub_6F4A60A0's hash of these three dwords is
//  constant across every agent and cannot be the source of a live
//  position-dependent desync; if something *does* reassign +0x44 to a real
//  value, that write has to happen in whichever base class (most likely
//  CPresence, given the concept, but not proven here) actually owns this
//  storage - CRlAgent's own ctor never touches it again after this initial
//  zero default.
//============================================================================
#ifndef CRLAGENT_H
#define CRLAGENT_H

//  Forward-declared rather than pulling in agentrelation.h's own full
//  declaration (Agent/agentrelation.h) - only a pointer parameter needs it
//  here.
struct SHandleRegistrationRequest;

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F4800C0 - NTempest::CDynTable<NIpse::CPrRelation*>'s own constructor
//  (`??_7?$CDynTable@PAVCPrRelation@NIpse@@@NTempest@@6B@`), the exact same
//  generic-container shape citemclosure_terrain_gates.cpp's own
//  ConstructTerrainLayerPriorityDynTable (0x6F746510,
//  CDynTable<TerrainLayerPriority>) already documents for a different
//  element type: stamp own vtable, call the polymorphic CMemBlock base
//  constructor (0x6F4B38B0, ConstructPolymorphicCMemBlock,
//  Misc/citemclosure_terrain_gates2.cpp - this repo has never built a
//  vtable for NTempest::CMemBlock, so that call bottoms out there), then
//  copy a caller-supplied {count, capacity} pair into +0x14/+0x18 and zero
//  +0x1C.  Out of scope for this session the same way the sibling
//  instantiation is - reused here by raw address exactly like
//  Net/fileconn_deleteself.cpp's own DestructFileConnFn/ConnectFinishFileFn
//  pattern, not re-declared as a second naked thunk.
//----------------------------------------------------------------------------
struct SDynTableInitPair
{
    unsigned int m_countArg;     // -> shifted left 2 and handed to the
                                  //    CMemBlock base ctor as an element count
    unsigned int m_capacity;     // -> copied verbatim into +0x14
};

struct CDynTable_CPrRelationPtr
{
    void*        m_vfptr;         // 0x00 - CDynTable<CPrRelation*>'s own vftable
    char         m_memBlockBase[0x10]; // 0x04-0x13 - CMemBlock/CEntity base part, unmodeled (see above)
    unsigned int m_capacity;      // 0x14
    unsigned int m_count;         // 0x18
    unsigned int m_reserved1C;    // 0x1C
};

//----------------------------------------------------------------------------
//  CRlAgent's own flat, offset-addressed layout (see the file comment above
//  for exactly which of these belong to the inlined base chain vs. to
//  CRlAgent itself).
//----------------------------------------------------------------------------
class CRlAgent
{
public:
    CRlAgent();
    ~CRlAgent();

    //  0x6F480370 - NIpse::CRlAgent's own vtable slot 1. Same
    //  unlink-then-return-to-pool shape as Agent/presence.h's own
    //  CPresence::ReturnToStaticPool/CPresenceTagged::ReturnToStaticPool and
    //  Agent/agentrelation.h's own CAgentRelation::ReturnToStaticPool,
    //  against this class's own static pool (CRlAgent_GetStaticPool,
    //  Agent/agentbasechain_staticpools.h).
    void ReturnToStaticPool();

    //  0x6F480410 - NIpse::CRlAgent's own vtable slot 2, scalar deleting
    //  destructor.  Unlike CPresence's/CPresenceTagged's/CPrRelation's own
    //  (Agent/presence.h's own header comment), this one DOES call a real
    //  base destructor first (~CRlAgent(), crlagent_dtor.cpp) and DOES carry
    //  the `this != 0` guard on the free - the same shape
    //  CAgentBaseAbs::ScalarDeletingDestructor
    //  (Agent/cagentbaseabs_module_batch2.cpp) and
    //  NTempest::CMemBlock::ScalarDeletingDestructor (Agent/cmemblock.h) use
    //  whenever a real destructor call precedes the free.
    void* ScalarDeletingDestructor(unsigned int flags);

    //  0x6F4A5AD0 - CAgentBaseAbs's own vtable slot 10 (+0x28,
    //  docs/targets/vtables/CAgentBaseAbs.txt).  Loads this class's own
    //  base (NIpse::CPrRelation, Agent/agentrelation.h's `CAgentRelation` -
    //  see that header's own Save/Load comment for why the two names are
    //  the same class) first, then this class's own added fields: the
    //  pending value at +0x50 read back as a path-ref entry, the
    //  related-agents table (`m_relatedAgents`, read entry by entry and
    //  inserted through `CRelatedAgentsTable::GrowInsert` -
    //  Agent/agentbaseabs_related.h, another parallel session's own batch
    //  of this same dump), and the four `SRelationLink` list heads at
    //  +0x78/+0x88/+0x98/+0xA8 (LoadRelationLink,
    //  Agent/agentbaseabsload.cpp).  Finally, if the +0x50 value read back
    //  non-null, fixes it up the same way CAgentBaseAbs's own vtable
    //  slot 3 does for a freshly-looked-up one
    //  (Agent/agentbaseabs_slot3.cpp's own `FixUp_6F4A45B0`, out of this
    //  session's own batch).
    void Load(void* reader);

    //  0x6F4A4D70 - CAgentBaseAbs's own vtable slot 3 (+0x0C) is where this
    //  is *reached*, but it is not that class's own override: CPresenceTagged
    //  overrides slot 3 first (0x6F4AEFF0), CPrRelation overrides it again
    //  (0x6F4A6070), and CRlAgent overrides it a *third* time with this
    //  address - CAgentBaseAbs never touches the slot again. See
    //  Agent/agentbaseabs_slot3.cpp for the body (unchanged by this rename;
    //  only the owning class changed).  Tells `arg` about this agent, looks
    //  up whatever `arg` resolves to and stamps the result into `this+0x50`.
    void Apply(SHandleRegistrationRequest* arg);

    //  0x6F4A5A70 - same story as Apply above, one level further down the
    //  slot-4 (+0x10) column: CPresenceTagged/CPrRelation both leave it a
    //  no-op, CRlAgent is the first real override, CAgentBaseAbs does not
    //  touch it again. See Agent/agentbaseabsrelatedcascade.cpp for the
    //  body. Sets the 0x1000000 flags bit, notifies, cascades to every
    //  related agent, unlinks this object from its own CLinkedNode-shaped
    //  list, zeroes +0x50/+0x54 and tears the relation down.
    void Slot4(int arg);

    //  0x6F4A4EE0 - CAgentBaseAbs's own vtable slot 9 (+0x24) reaches this,
    //  but CRlAgent is the real owner (CPresenceTagged/CPrRelation both
    //  leave slot 9 a no-op; CAgentBaseAbs never overrides it again). Also
    //  folds in what used to be the separate `CAgentBaseAbsRelated::Save` -
    //  same address, same body, moved here because every field it touches
    //  (`m_relatedAgents` at +0x58, the four `SRelationLink` heads at
    //  +0x78/+0x88/+0x98/+0xA8) is CRlAgent's own, not CAgentBaseAbs's - see
    //  Agent/agentbaseabssave.cpp.
    void Save(void* store);

    //  0x6F4A5A30 - CPrRelation *introduces* vtable slot 17 as a no-op
    //  (nullsub_1486 - it does not exist at all in CPresenceTagged, which
    //  has only 17 slots); CRlAgent is the first class to give it a real
    //  body, and CAgentBaseAbs does not override it again. See
    //  Agent/agentbaseabsslot17.cpp.
    void Slot17(void* arg);

    //  ---- former CAgentBaseAbsRelated members (Agent/agentbaseabs_related.h) ----
    //  All of these operate on this class's own added fields
    //  (`m_relatedAgents` at +0x58, or the four `SRelationLink` list heads
    //  at +0x78/+0x88/+0x98/+0xA8) and were originally scoped under
    //  `CAgentBaseAbs` on the mistaken assumption that "one of
    //  CAgentBaseAbs's 19 vtable slots" implied "CAgentBaseAbs's own
    //  method" - they are plain non-virtual helpers, reached only through
    //  DispatchOrder's switch (Agent/agentbaseabsorderdispatch.cpp), and
    //  every field they touch belongs to CRlAgent (see this file's own
    //  header comment and the m_relatedAgents field below), so they are
    //  real members of CRlAgent instead.  Bodies unchanged - see
    //  Agent/agentbaseabs_relatedaccessors.cpp and
    //  Agent/agentbaseabssubmitevent.cpp.

    //  0x6F4A50F0 / 0x6F4A5120 / 0x6F4A5150 - if `index` names a live
    //  related agent, thread it onto this agent's own kind-1/2/3 list
    //  (`+0x88`/`+0x98`/`+0xA8`) via `CAgentRelation::AttachEndpointA`, with
    //  this agent itself as the endpoint.
    void AttachKind1(unsigned int index);
    void AttachKind2(unsigned int index);
    void AttachKind3(unsigned int index);

    //  0x6F4A51E0 - dispatch by `kind` (0-3): kind 0 is the same shape as
    //  the three above, inlined against `+0x78` rather than calling out;
    //  kinds 1-3 tail-call the three methods above. Any other kind is a
    //  silent no-op (the switch has no default and nothing follows it).
    void Attach(unsigned int index, unsigned int kind);

    //  0x6F4A5250 - if `index` names a live related agent, detach it
    //  (`DetachRelation`, Agent/agentrelationdetachboth.cpp).
    void DetachIndex(unsigned int index, int alsoEndpointB);

    //  0x6F4A5280 - if `index` names a live related agent, detach its
    //  endpoint B only if it is actually linked (`DetachEndpointBIfLinked`,
    //  Agent/agentrelationdetachboth.cpp).
    void DetachIndexEndpointBIfLinked(unsigned int index);

    //  0x6F4A52B0 - if `index` names a live related agent, stamp its
    //  `m_eventId` (`+0x48`, agentrelation.h) with `value`.
    void SetEventId(unsigned int index, unsigned int value);

    //  0x6F4A53B0 / 0x6F4A53E0 - if `index` names a live related agent,
    //  attach ITS endpoint B to `value` (an arbitrary object, not
    //  necessarily another related agent of this same table) via
    //  `CAgentRelation::AttachEndpointB`, handing `value`'s own `+0x50`
    //  (kind 2) or `+0x60` (kind 3, not otherwise named in this dump) as
    //  the `SRelationLink` list to thread onto.
    void SetField2(unsigned int index, void* value);
    void SetField3(unsigned int index, void* value);

    //  0x6F4A57A0 - resolve `linkIndex` to a live related agent (returning
    //  with no effect if it names none), then dispatch to `SetField2`/
    //  `SetField3` by `kind` (2 or 3 only - any other value is a no-op)
    //  with `targetIndex` and the resolved agent as `SetField2`/3's own
    //  `index`/`value`.
    void SetField(unsigned int targetIndex, unsigned int linkIndex,
                 unsigned int kind);

    //  0x6F4A5710 - build an 11-dword `SWidgetAgentQuery`-shaped context
    //  (Widget/widgetagentquery.h) by copying `*ctx`, then overwriting its
    //  `+0x24`/`+0x28` fields per the gate described in
    //  Agent/agentbaseabssubmitevent.cpp, submits it via
    //  `SubmitWidgetAgentQuery` and inserts the result as a new related
    //  agent via `CRelatedAgentsTable::GrowInsert`.
    void SubmitRelatedAgentEvent(const void* ctx, const void* table);

    //  ---- inlined base-chain fields (CPresence/CPresenceTagged/CPrRelation) ----
    void*        m_vfptr;              // 0x00 - CPrRelation's vftable, then overwritten with CRlAgent's own
    unsigned int m_baseReserved04[4];  // 0x04-0x10, zero-inited
    unsigned int m_baseFreeIndex14;    // 0x14 - -1, "no slot" sentinel
    unsigned int m_baseFreeIndex18;    // 0x18 - -1, same shape as +0x14
    unsigned int m_baseReserved1C[3];  // 0x1C-0x24, zero-inited
    unsigned int m_baseReserved28;     // 0x28, zero-inited
    void*        m_baseListPrev;       // 0x2C - self-pointer, empty CLinkedNode-shaped list head
    unsigned int m_baseReserved30[3];  // 0x30-0x38, zero-inited
    void*        m_baseListNext;       // 0x3C - self-pointer, same shape as +0x2C
    unsigned int m_baseReserved40;     // 0x40, zero-inited
    unsigned int m_baseDefaultFloat44; // 0x44 - copies g_CFloatZero's own bit pattern (0x00000000, a CFloat
                                        // field, not a raw literal - the shipped code dereferences the
                                        // global rather than embedding an immediate 0); NOT CRlAgent's own field
    unsigned int m_baseSentinel48;     // 0x48 - -1, same shape as +0x14/+0x18/+0x48
    unsigned int m_baseReserved4C;     // 0x4C, zero-inited

    //  ---- CRlAgent's own added fields (written after the 0x480262 restamp) ----
    unsigned int m_ownReserved50;      // 0x50, zero-inited
    unsigned int m_ownReserved54;      // 0x54, zero-inited
    CDynTable_CPrRelationPtr m_relatedAgents; // 0x58, capacity 8
    char         m_ownReserved78[0x40]; // 0x78-0xB7, zero-inited
};

//----------------------------------------------------------------------------
//  0x6F480320 - CRlAgent's own vtable slot 0 (confirmed by a direct read of
//  the vtable's own first dword in agent_worktrees/names.json, and now also
//  by the full CRlAgent vtable entry at agent_worktrees/classes/0x6F95207C.json,
//  19 slots, from the operator's 2026-09-07 re-dump).  A trivial "class
//  name" accessor: returns the literal string "RlAgent" (aRlagent,
//  0x6F9520C8, immediately after this vtable's own data) and nothing else.
//----------------------------------------------------------------------------
const char* CRlAgent_GetTypeName();

}  // namespace NIpse

#endif  // CRLAGENT_H
