//============================================================================
//  NIpse::CPoLink (`??_7CPoLink@NIpse@@6B@`, 0x6F95302C, 19 slots) - cluster
//  F's follow-up session, 2026-09-08 (see cpoflag.h's own file header for
//  the shared "plain subset" notes - no slot-8 override, idx4 is the same
//  shared 0x6F4975C0 a different concurrent session already reconstructed).
//
//  Layout: extends `NIpse::CRlProperty` (Agent/rlproperty.h, offsets
//  0x00-0x74) with a two-dword link-target pair at +0x78/+0x7C - a target
//  handle and its own sub-index, per the "\ttgt t[%u] b[%u]" DumpDebugState
//  line - plus a third dword at +0x80 that Save/Load stream but neither
//  Apply, ComputeChecksum nor DumpDebugState ever touch in this call tree
//  (meaning not established here; nothing in this cluster's own scope
//  writes it, so it is declared but left unnamed-by-purpose).
//
//  own new slots (this session's batch):
//    idx0  0x6F48D210  GetTypeName            -> "PoLink"
//    idx1  0x6F48D390  ReturnToStaticPool      (own pool, CPoLink_GetStaticPool)
//    idx2  0x6F48D3E0  ScalarDeletingDestructorTrivial (restamp-to-CPresence triplet)
//    idx3  0x6F4AA130  Apply   - CRlProperty::Apply(arg), then a lookup+fixup:
//                      on a hit, m_targetHandle/m_targetSub = lookup's own
//                      +0x14/+0x18; on a miss, both -1.
//    idx9  0x6F4AA170  Save    - CRlProperty::Save(writer), then writes
//                      m_targetHandle/m_targetSub
//    idx10 0x6F4AA1B0  Load    - CRlProperty::Load(reader), then reads
//                      m_targetHandle/m_targetSub
//    idx15 0x6F48D220  ComputeChecksum - CRlProperty::ComputeChecksum(hash),
//                      then folds both fields into *hash
//    idx16 0x6F48D2D0  DumpDebugState  - CRlProperty::DumpDebugState(sink),
//                      then "[Link]" / "\ttgt t[%u] b[%u]"
//============================================================================
#ifndef CPOLINK_H
#define CPOLINK_H

struct SHandleRegistrationRequest;

namespace NIpse {

class CPoLink
{
public:
    //  0x6F48D210 - vtable slot 0. Literal "PoLink" (aPolink).
    const char* GetTypeName();

    //  0x6F48D390 - vtable slot 1. Own static pool, CPoLink_GetStaticPool.
    void ReturnToStaticPool();

    //  0x6F48D3E0 - vtable slot 2, scalar deleting destructor. Same
    //  restamp-to-CPresence triplet as the rest of this family.
    void* ScalarDeletingDestructorTrivial(unsigned int flags);

    //  0x6F4AA130 - vtable slot 3. CRlProperty::Apply(arg), then a lookup
    //  through `arg` (SPoLinkTargetSource::QueryTarget below, own +0x0C
    //  field, null-safe) - on a hit, copies the lookup result's own
    //  +0x14/+0x18 fields into m_targetHandle/m_targetSub; on a miss
    //  (null lookup), sets both to -1.
    void Apply(SHandleRegistrationRequest* arg);

    //  0x6F4AA170 / 0x6F4AA1B0 - vtable slots 9/10. CRlProperty::Save/Load
    //  first, then this class's own m_targetHandle, m_targetSub and
    //  m_field80, in that offset order.
    void Save(void* writer);
    void Load(void* reader);

    //  0x6F48D220 - vtable slot 15. CRlProperty::ComputeChecksum(hash) via
    //  the base, then folds m_targetHandle then m_targetSub into `*hash`.
    void ComputeChecksum(unsigned int* hash);

    //  0x6F48D2D0 - vtable slot 16. CRlProperty::DumpDebugState(sink) via
    //  the base (inlined - see .cpp), then "[Link]" and
    //  "\ttgt t[%u] b[%u]" with t=m_targetHandle (+0x78), b=m_targetSub
    //  (+0x7C).
    void DumpDebugState(void* sink);

    //  ---- base-chain territory (CRlProperty and below) - see
    //  Agent/rlproperty.h - plus this class's own added fields.
    char         m_baseReserved00[0x78];
    unsigned int m_targetHandle;  // +0x78
    unsigned int m_targetSub;     // +0x7C
    unsigned int m_field80;       // +0x80 - Save/Load only, see file header
};

}  // namespace NIpse

#endif  // CPOLINK_H
