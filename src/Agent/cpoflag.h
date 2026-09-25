//============================================================================
//  NIpse::CPoFlag (`??_7CPoFlag@NIpse@@6B@`, 0x6F952FBC, 19 slots) - one of
//  the seven remaining siblings of the "Po-modifier/listener" family
//  (cluster F's own follow-up session, 2026-09-08 - see
//  docs/targets/NTempestNIpse_PoModifierListener_ClusterF.md for the full
//  9-class family and the two, CPoLkLis/CPoFgLis, an earlier session
//  finished).
//
//  Part of the "plain" subset (CPoFlag/CPoLink/CPoReal) this cluster's own
//  doc already flags: no slot-8 override (inherits the shared no-op there),
//  and its own idx4 is the SAME shared address (0x6F4975C0,
//  Agent/rlproperty_slot4.cpp's own `NIpse::CRlProperty::Slot4`) a different
//  concurrent session (`claude-ntempest-cluster-a`) already reconstructed -
//  not touched again here.
//
//  Layout: this class extends `NIpse::CRlProperty` (Agent/rlproperty.h,
//  offsets 0x00-0x74) with exactly one new field of its own, a plain dword
//  at +0x78 - the "val[0x%X]" DumpDebugState prints, what ComputeChecksum
//  folds, and what Save/Load stream. Reached through an explicit
//  `(CRlProperty*)this`/`(CAgentRelation*)this` cast rather than real C++
//  inheritance, matching this whole base chain's own flat/offset-addressed
//  convention (Pathfinding/crlagent.h's own file header).
//
//  own new slots (this session's batch):
//    idx0  0x6F48D010  GetTypeName            -> "PoFlag"
//    idx1  0x6F48D140  ReturnToStaticPool      (own pool, CPoFlag_GetStaticPool)
//    idx2  0x6F48D190  ScalarDeletingDestructorTrivial (restamp-to-CPresence triplet)
//    idx3  0x6F4A8A00  Apply   - CRlProperty::Apply(arg), m_value = query(arg)
//    idx9  0x6F4A8A20  Save    - CRlProperty::Save(writer), then writes m_value
//    idx10 0x6F4A8A40  Load    - CRlProperty::Load(reader), then reads m_value
//    idx15 0x6F48D020  ComputeChecksum - CRlProperty::ComputeChecksum(hash),
//                      then folds m_value's own four bytes into *hash
//    idx16 0x6F48D080  DumpDebugState  - CRlProperty::DumpDebugState(sink),
//                      then "[Flag]" / "\tval[0x%X]" of m_value
//============================================================================
#ifndef CPOFLAG_H
#define CPOFLAG_H

struct SHandleRegistrationRequest;

namespace NIpse {

class CPoFlag
{
public:
    //  0x6F48D010 - vtable slot 0. Literal "PoFlag" (aPoflag).
    const char* GetTypeName();

    //  0x6F48D140 - vtable slot 1. Same unlink-then-return-to-pool shape as
    //  every sibling in this family, against this class's own static pool
    //  (CPoFlag_GetStaticPool, Agent/agentbasechain_staticpools.h/.cpp).
    void ReturnToStaticPool();

    //  0x6F48D190 - vtable slot 2, scalar deleting destructor. Same
    //  restamp-to-CPresence-vftable triplet shape as the rest of this
    //  family (Agent/presence.h's own file header).
    void* ScalarDeletingDestructorTrivial(unsigned int flags);

    //  0x6F4A8A00 - vtable slot 3. Tail-reaches CRlProperty::Apply
    //  (Agent/rlproperty.h, `this` reinterpreted the same layout-compatible
    //  way rlproperty_apply.cpp's own body already establishes for
    //  CPathTrace), then queries a value off the incoming
    //  SHandleRegistrationRequest* (its own +0x0C field, null-safe, 0 on a
    //  null request - see cpoflag_queryvalue.cpp) into this class's own
    //  m_value.
    void Apply(SHandleRegistrationRequest* arg);

    //  0x6F4A8A20 / 0x6F4A8A40 - vtable slots 9/10. CRlProperty::Save/Load
    //  first, then this class's own m_value via
    //  CDataStoreScratch::WriteDwordAlt2 / CDataStore::ReadDwordAlt3.
    void Save(void* writer);
    void Load(void* reader);

    //  0x6F48D020 - vtable slot 15. CRlProperty::ComputeChecksum(hash) via
    //  the base, then folds m_value's own four bytes into `*hash`.
    void ComputeChecksum(unsigned int* hash);

    //  0x6F48D080 - vtable slot 16. CRlProperty::DumpDebugState(sink) via
    //  the base (inlined - see .cpp), then this object's own "[Flag]" tag
    //  and m_value as "\tval[0x%X]".
    void DumpDebugState(void* sink);

    //  ---- base-chain territory (CRlProperty and below, offsets 0x00-0x77)
    //  - not modelled here, see Agent/rlproperty.h - plus this class's own
    //  added field.
    char         m_baseReserved00[0x78];
    unsigned int m_value;   // +0x78
};

}  // namespace NIpse

#endif  // CPOFLAG_H
