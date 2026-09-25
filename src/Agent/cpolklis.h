//============================================================================
//  NIpse::CPoLkLis (`??_7CPoLkLis@NIpse@@6B@`, 0x6F952D94, 19 slots) - one of
//  nine sibling "Po-modifier/listener" classes branching off
//  NIpse::CPrRelation/CAgentRelation (Agent/agentrelation.h) - see
//  docs/targets/NTempestNIpse_PoModifierListener_ClusterF.md for the full
//  9-class family and which ones this session actually reconstructed.
//
//  Adds NO fields of its own: its object layout is exactly
//  CAgentRelation's (this session found no store past +0x54 anywhere in its
//  own 8 new slots), and it inherits slots 15/16 (ComputeChecksum/
//  DumpDebugState) completely unchanged from CAgentRelation - the only one
//  of the nine siblings that does, per the cluster's own task notes. Kept
//  flat/offset-addressed rather than real C++ inheritance, matching
//  Pathfinding/crlagent.h's own precedent for this exact base chain: the
//  layout is hand-verified per field, not derived from a real base
//  sub-object, so a genuine `: public CAgentRelation` would let the
//  compiler pick its own layout instead of the one already confirmed here.
//  Slots reached that are genuinely CAgentRelation's own unchanged code
//  (ResetEventBinding, Teardown, Save, Load) are called through an explicit
//  `(CAgentRelation*)this` cast, the same pattern
//  Agent/agentbaseabs_slot3.cpp's own `CRlAgent::Apply` already establishes
//  for this same base method.
//
//  own new slots (this session's batch):
//    idx0  0x6F48C2D0  GetTypeName            -> "PoLkLis"
//    idx1  0x6F48C330  ReturnToStaticPool      (own pool, CPoLkLis_GetStaticPool)
//    idx2  0x6F48C370  ScalarDeletingDestructorTrivial (restamp-to-CPresence triplet)
//    idx3  0x6F4AA230  Slot3   - ResetEventBinding(arg), then flags |= 0x400
//    idx4  0x6F4AA040  Slot4   - tail-calls CAgentRelation::Teardown(arg) directly
//    idx8  0x6F4AA250  NotifyEndpointAIfTagged - REAL override, unlike the
//                      "*Flag"/"*Link"/"*Real" plain subset of this family,
//                      which inherit slot 8 as the shared no-op instead (see
//                      the cluster doc's own structural-split note)
//    idx9  0x6F4AA050  Save    - tail-calls CAgentRelation::Save(writer)
//    idx10 0x6F4AA060  Load    - tail-calls CAgentRelation::Load(reader)
//============================================================================
#ifndef CPOLKLIS_H
#define CPOLKLIS_H

struct SHandleRegistrationRequest;

namespace NIpse {

class CPoLkLis
{
public:
    //  0x6F48C2D0 - vtable slot 0. `retn` with zero stack bytes, no `this`
    //  read - the literal string "PoLkLis" (aPolklis).
    const char* GetTypeName();

    //  0x6F48C330 - vtable slot 1. Same unlink-then-return-to-pool shape as
    //  Agent/agentrelation.h's own CAgentRelation::ReturnToStaticPool and
    //  Pathfinding/crlagent.h's own CRlAgent::ReturnToStaticPool, against
    //  this class's own static pool (CPoLkLis_GetStaticPool, below).
    void ReturnToStaticPool();

    //  0x6F48C370 - vtable slot 2, scalar deleting destructor. Same
    //  byte-identical restamp-to-CPresence-vftable, no-base-teardown,
    //  no-`this!=0`-guard shape Agent/presence.h's own file header
    //  documents for CPresence/CPresenceTagged/CAgentRelation's own three -
    //  a fourth address sharing the exact same body.
    void* ScalarDeletingDestructorTrivial(unsigned int flags);

    //  0x6F4AA230 - vtable slot 3. Calls straight into CAgentRelation's own
    //  ResetEventBinding (Agent/agentrelation.h, reached the same way
    //  Agent/agentbaseabs_slot3.cpp's own CRlAgent::Apply already reaches
    //  it), then sets a flags bit this call tree does not otherwise name
    //  (0x400 - no existing kRel* constant in agentrelation.h matches it).
    void Slot3(SHandleRegistrationRequest* arg);

    //  0x6F4AA040 - vtable slot 4. The entire shipped body is one `jmp`
    //  straight into CAgentRelation::Teardown (0x6F4A6920) with matching
    //  argument count/`retn 4` - a genuine override that replaces the slot
    //  with the base's own teardown rather than adding anything of its own.
    void Slot4(int arg);

    //  0x6F4AA250 - vtable slot 8. The REAL override this class (and the
    //  rest of the "*Lis"/"*Mod" listener/modifier subset of this family)
    //  has that the plain "*Flag"/"*Link"/"*Real" subset does not (see this
    //  header's own file comment). Naked: see cpolklis_notify.cpp for why.
    void* NotifyEndpointAIfTagged(const void* arg);

    //  0x6F4AA050 - vtable slot 9. Tail-calls CAgentRelation::Save
    //  unchanged (same argument, same `retn 4`).
    void Save(void* writer);

    //  0x6F4AA060 - vtable slot 10. Tail-calls CAgentRelation::Load
    //  unchanged (same argument, same `retn 4`).
    void Load(void* reader);
};

}  // namespace NIpse

//  0x6F4AA1F0 - NIpse::CPoLkLis's own static pool: g_pTimeSync + 0x8F8.
//  Declared (and defined) in Agent/agentbasechain_staticpools.h/.cpp
//  alongside its siblings, NOT namespaced under NIpse - same global-scope
//  convention every other getter in that file already uses. Kept out of
//  this header (rather than re-declared here) so there is exactly one
//  declaration to go out of sync with the definition.

#endif  // CPOLKLIS_H
