//============================================================================
//  NIpse::CPoReThresholdLis (`??_7CPoReThresholdLis@NIpse@@6B@`,
//  0x6F952B6C, 19 slots) - cluster F's follow-up session, 2026-09-08.
//  Listener/modifier subset (real slot-8 override in principle - see
//  "Not attempted" note below), reaches CAgentRelation directly.
//
//  Layout: extends `NIpse::CAgentRelation` (offsets 0x00-0x4F) with
//  m_threshold (+0x50, CFloat) and m_mode (+0x54, dword).
//
//  own new slots (this session's batch):
//    idx0  0x6F48B4B0  GetTypeName            -> "PoReThresholdLis"
//    idx1  0x6F48B610  ReturnToStaticPool      (own pool, CPoReThresholdLis_GetStaticPool)
//    idx2  0x6F48B650  ScalarDeletingDestructorTrivial (restamp-to-CPresence triplet)
//    idx3  0x6F4A9A60  Apply   - ResetEventBinding(arg), m_threshold/m_mode
//                      from `arg` (0x0C/0x10), flags |= 0x90400
//    idx4  0x6F4A8F50  Slot4   - tail-calls CAgentRelation::Teardown(arg)
//    idx8  0x6F4A9D80  NOT reconstructed this session - its own body pulls
//                      in a deeper transitive chain (sub_6F4A9CF0 ->
//                      sub_6F4A9940/sub_6F4A9B70, sub_6F4771E0) that goes
//                      past this cluster's own 9-slot scope; left TODO,
//                      unclaimed, for a future session
//    idx9  0x6F4A9AA0  Save    - CAgentRelation::Save(writer), then writes
//                      m_threshold/m_mode
//    idx10 0x6F4A9AD0  Load    - CAgentRelation::Load(reader), mirror
//    idx15 0x6F48B4C0  ComputeChecksum - folds m_threshold then m_mode
//    idx16 0x6F48B570  DumpDebugState  - "[ReThrLis]" / "\tthr[%s] mod[%10u]"
//============================================================================
#ifndef CPORETHRESHOLDLIS_H
#define CPORETHRESHOLDLIS_H

#include "CFloat.h"

struct SHandleRegistrationRequest;

//  0x6F4A9170 / 0x6F4A9190 - see cporethresholdlis_reqfields.cpp's own
//  file header for why these are free functions rather than members.
CFloat* __fastcall CopyRequestFieldToCFloat0x10(CFloat* dest, const void* request);
CFloat* __fastcall CopyRequestFieldToCFloat0x14(CFloat* dest, const void* request);

namespace NIpse {

class CPoReThresholdLis
{
public:
    const char* GetTypeName();
    void ReturnToStaticPool();
    void* ScalarDeletingDestructorTrivial(unsigned int flags);
    void Apply(SHandleRegistrationRequest* arg);
    void Slot4(int arg);

    //  0x6F4A9D80 - vtable slot 8. See cporethresholdlis_notify.cpp.
    void* NotifyEndpointAIfTagged(const void* arg);

    //  0x6F4A9940 - shared hysteresis test this class's own slot 8 uses
    //  twice (once per candidate point): is `a`/`b` within g_driveTolerance
    //  of this class's own m_threshold, with a small dead-zone so the
    //  state does not chatter right at the boundary. `this` is reached
    //  through the identical +0x50/+0x54 storage
    //  NIpse::CDrivenRelation::AtTarget (agentrelation.h) already compares
    //  against as m_target/m_mode - see cporethresholdlis_notify.cpp's own
    //  file header.
    int CompareAgainstThreshold(const CFloat* a, const CFloat* b);

    //  0x6F4A9CF0 - see cporethresholdlis_announce.cpp.
    void AnnounceCrossing(const CFloat* a, const CFloat* b);

    void Save(void* writer);
    void Load(void* reader);
    void ComputeChecksum(unsigned int* hash);
    void DumpDebugState(void* sink);

    char         m_baseReserved00[0x50];
    CFloat       m_threshold;   // +0x50 (CFloatZero default off `arg`+0x0C)
    unsigned int m_mode;        // +0x54 (dword, 0 default off `arg`+0x10)
};

}  // namespace NIpse

#endif  // CPORETHRESHOLDLIS_H
