//============================================================================
//  NIpse::CRlProperty (`??_7CRlProperty@NIpse@@6B@`, 0x6F95242C) - one of the
//  two siblings this session's own dump adds beside NIpse::CRlAgent
//  (Pathfinding/crlagent.h) at the base of the CAgentBaseAbs chain: real,
//  RTTI-confirmed classes with no header before this session
//  (NTempestNIpse_BaseChain cluster A, 2026-09-08 - see
//  docs/targets/NTempestNIpse_BaseChain.md and this batch's own
//  asm/NTempestNIpse_clusterA_calltree_asm.md).
//
//  Per the task's own slot-by-slot cross-reference against
//  agent_worktrees/classes/0x6F95242C.json: slots 5-8, 11-14 and 17 are the
//  shared no-ops every class in this family inherits unchanged
//  (SVtableNoOp/SVtableNoOpArg), and slot 18 is the already-established
//  `CAgentRelation::OnRegistrationRetired` (agentrelation.h) - CRlProperty
//  never overrides it. Slots 0, 1, 2, 3, 4, 9, 10, 15, 16 are this class's
//  own, first introduced here (its base, whatever it is below
//  NTempest::CPresenceTagged, leaves every one of them a no-op or, for 15,
//  the CPresenceTagged-level `FoldLinkFieldsIntoHash`/CPrRelation-level
//  `ComputeChecksum` this class further overrides).
//
//  No real C++ layout is modelled here beyond what this call tree touches -
//  see Pathfinding/crlagent.h's own file header for why this whole chain
//  stays flat and address-only. This class's own base-chain portion
//  (offsets 0x00-0x4F, inherited unchanged from whatever it derives from)
//  is not re-documented per field here; only what THIS class's own methods
//  read or write is named.
//============================================================================
#ifndef RLPROPERTY_H
#define RLPROPERTY_H

struct SHandleRegistrationRequest;

namespace NIpse {

class CRlProperty
{
public:
    //  0x6F487430 - vtable slot 0. `retn` with zero stack bytes, no `this`
    //  read - the literal string "RlProperty" (aRlproperty, 0x6F952478,
    //  immediately after this vtable's own data), same shape
    //  Pathfinding/crlagent.h's own `CRlAgent_GetTypeName` establishes one
    //  level up this chain.
    const char* GetTypeName();

    //  0x6F487580 - vtable slot 1. Same unlink-then-return-to-pool shape as
    //  every sibling in this chain (Agent/presence.h,
    //  Pathfinding/crlagent.h), against this class's own static pool
    //  (CRlProperty_GetStaticPool, agentbasechain_staticpools.h).
    void ReturnToStaticPool();

    //  0x6F4875C0 - vtable slot 2, scalar deleting destructor. Named
    //  `ScalarDeletingDestructorTrivial` - a fifth instance of the
    //  byte-identical-apart-from-address triplet Agent/presence.h's own
    //  header comment documents for CPresence/CPresenceTagged/CPrRelation:
    //  no base teardown call, no `this != 0` guard, restamps straight to
    //  `??_7CPresence@NTempest@@6B@` regardless of this class's own vtable.
    void* ScalarDeletingDestructorTrivial(unsigned int flags);

    //  0x6F4975A0 - vtable slot 3. Tells this object's own embedded
    //  CAgentRelation about `arg` (CAgentRelation::ResetEventBinding,
    //  agentrelation.h), then tail-calls the SAME leaf address
    //  Pathfinding/CPathTrace2.cpp's own `CPathTrace::RefreshSyncSample`
    //  already owns (0x6F497440, EXACT there, cross-dump reused per
    //  tools/funcmap.py's own BEHAVIOUR note on that address) against
    //  `this` reinterpreted the same layout-compatible way. Named `Apply`
    //  to match the sibling slot-3 column's own naming
    //  (Pathfinding/crlagent.h's `CRlAgent::Apply`), a simpler override
    //  with no lookup/fixup step of its own.
    void Apply(SHandleRegistrationRequest* arg);

    //  0x6F4975C0 - vtable slot 4. Sets the 0x1000000 flags bit (+0x4C,
    //  same bit Pathfinding/crlagent.h's own `CRlAgent::Slot4` sets),
    //  detaches every relation still threaded onto this object's own two
    //  dependency lists (`DetachDependents`, sub_6F4974A0 - see .cpp), then
    //  tears the underlying relation down (CAgentRelation::Teardown).
    void Slot4(int arg);

    //  0x6F4975E0 - vtable slot 9 (+0x24). This class's own save: the
    //  embedded CAgentRelation's own Save first, then this class's own two
    //  relation-link handles (+0x50/+0x60, via
    //  `SaveLinkRefIndirect` - agentrelationsavelinkrefindirect.cpp) and its
    //  own CFloat/dword pair at +0x70/+0x74.
    void Save(void* writer);

    //  0x6F497620 - vtable slot 10 (+0x28), Load's exact mirror image.
    void Load(void* reader);

    //  0x6F487440 - vtable slot 15 (+0x3C). Folds the base
    //  `CAgentRelation::ComputeChecksum` (agentrelation.h) first, then this
    //  class's own two added dwords at +0x70/+0x74, using the identical
    //  per-byte `(b*0x7A2D)^b` fold idiom
    //  Agent/agentbaseabscomputechecksum.cpp already names (kept as its own
    //  local copy here for the same "no shared translation unit" reason
    //  that file gives).
    void ComputeChecksum(unsigned int* hash);

    //  0x6F4874F0 - vtable slot 16 (+0x40). Dumps the base
    //  `CAgentRelation::DumpDebugState` first, then this object's own value
    //  name, a "[Prop]" tag and its own base/index pair, all through the
    //  same debug text sink (Storm/textsink.h) - see .cpp.
    void DumpDebugState(void* sink);

    //  0x6F4974A0 - walk this object's own two dependency lists (the raw
    //  head pointers at +0x54/+0x64, each threaded through a
    //  `SRelationEndpointSlot`-shaped node - agentrelationsyncsave.cpp's
    //  own struct) and detach each one's own owning relation
    //  (CAgentRelation::DetachEndpointB) until the head goes null - the
    //  callee is what advances the head, not this loop.
    //
    //  Widened from `private` to `public` (Agent/popos.h's own "cluster H
    //  remainder" session, 2026-09-08): NIpse::CPoPos::Slot4 (poposbh.h's
    //  own follow-up) calls the SAME address as a real base-class call,
    //  reached the same `((CRlProperty*)this)->DetachDependents()`
    //  reinterpret-cast way this file's own CRlProperty::Apply already
    //  reaches CAgentRelation's methods - NOT by real C++ inheritance
    //  (CPoPos is a flat, offset-addressed class like crlagent.h's own
    //  CRlAgent, not `: public CRlProperty` - CRlProperty itself carries
    //  no data members, so inheriting from it would place CPoPos's own
    //  +0x78-only fields at the wrong, empty-base-optimised offset 0
    //  instead of the shipped +0x78; discovered live by this session's own
    //  0.75-score Slot4 mismatch before this fix). A cast-based call to a
    //  `private` method does not compile, so `public` it is - matching how
    //  every other cross-class call in this whole file already works.
    void DetachDependents();
};

}  // namespace NIpse

#endif  // RLPROPERTY_H
