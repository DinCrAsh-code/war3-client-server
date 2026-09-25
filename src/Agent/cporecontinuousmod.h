//============================================================================
//  NIpse::CPoReContinuousMod (`??_7CPoReContinuousMod@NIpse@@6B@`,
//  0x6F952A4C, 19 slots) - cluster F's follow-up session, 2026-09-08. Part
//  of the "listener/modifier" subset (like NIpse::CPoLkLis/CPoFgLis,
//  Agent/cpolklis.h/cpofglis.h - the two classes an earlier session
//  finished): overrides vtable slot 8 for real, with a `'^rel'`-tagged
//  filter/react body, and reaches CAgentRelation directly (no CRlProperty
//  middle layer the "plain" CPoFlag/CPoLink/CPoReal subset goes through).
//
//  Layout: extends `NIpse::CAgentRelation` (Agent/agentrelation.h, offsets
//  0x00-0x4F) with two CFloats of its own: m_rate at +0x50, m_duration at
//  +0x54.
//
//  own new slots (this session's batch):
//    idx0  0x6F48AF20  GetTypeName            -> "PoReContinuousMod"
//    idx1  0x6F48B090  ReturnToStaticPool      (own pool, CPoReContinuousMod_GetStaticPool)
//    idx2  0x6F48B0D0  ScalarDeletingDestructorTrivial (restamp-to-CPresence triplet)
//    idx3  0x6F4A95B0  Apply   - ResetEventBinding(arg), m_rate/m_duration
//                      from `arg`'s own +0x10/+0x14 (CFloatZero default),
//                      flags |= 0x40044
//    idx4  0x6F4A8EB0  Slot4   - tail-calls CAgentRelation::Teardown(arg)
//    idx8  0x6F4A9F80  NOT reconstructed this session: calls
//                      sub_6F4A9320 -> sub_6F4A91B0, a substantial,
//                      unreconstructed event-context builder well past
//                      this cluster's own 9-slot scope (unlike the other
//                      three "mod/lis" siblings' own idx8, whose
//                      transitive callees were already reconstructed or
//                      trivial). Left TODO, unclaimed, for a future
//                      session.
//    idx9  0x6F4A8EC0  Save    - CAgentRelation::Save(writer), then writes
//                      m_rate/m_duration
//    idx10 0x6F4A8EF0  Load    - CAgentRelation::Load(reader), mirror
//    idx15 0x6F48AF30  ComputeChecksum - CAgentRelation::ComputeChecksum(hash),
//                      then folds m_rate/m_duration into *hash
//    idx16 0x6F48AFE0  DumpDebugState  - CAgentRelation::DumpDebugState(sink),
//                      then "[ReContMod]" / "\trte[%s] dur[%s]"
//============================================================================
#ifndef CPORECONTINUOUSMOD_H
#define CPORECONTINUOUSMOD_H

#include "CFloat.h"

struct SHandleRegistrationRequest;

namespace NIpse {

class CPoReContinuousMod
{
public:
    //  0x6F48AF20 - vtable slot 0. Literal "PoReContinuousMod".
    const char* GetTypeName();

    //  0x6F48B090 - vtable slot 1. Own static pool,
    //  CPoReContinuousMod_GetStaticPool.
    void ReturnToStaticPool();

    //  0x6F48B0D0 - vtable slot 2, scalar deleting destructor.
    void* ScalarDeletingDestructorTrivial(unsigned int flags);

    //  0x6F4A95B0 - vtable slot 3. ResetEventBinding(arg) via the base,
    //  then m_rate/m_duration from `arg`'s own +0x0C/+0x10 fields (via the
    //  new leaves PoReContinuousModQueryRate/QueryDuration - null-safe,
    //  CFloatZero default), then flags |= 0x40044.
    void Apply(SHandleRegistrationRequest* arg);

    //  0x6F4A8EB0 - vtable slot 4. Single-`jmp` tail call into
    //  CAgentRelation::Teardown, same shape as Agent/cpolklis_slot4.cpp's
    //  own CPoLkLis::Slot4.
    void Slot4(int arg);

    //  0x6F4A9F80 - vtable slot 8. Naked - see cporecontinuousmod_notify.cpp.
    void* NotifyEndpointAIfTagged(const void* arg);

    //  0x6F4A9E70 - own leaf NotifyEndpointAIfTagged's own '^beg' arm
    //  reaches: arm (or clear) the duration channel registration against
    //  `target`, using g_unk6FAAE624 (Math/floatmini.h, the shared
    //  "unbounded" high-bound constant) as the "still open" test and a
    //  reserved sentinel CFloat when it is. See cporecontinuousmod_arm.cpp.
    void ArmDurationRegistration(const CFloat* target);

    //  0x6F4A8EC0 / 0x6F4A8EF0 - vtable slots 9/10. CAgentRelation::Save/
    //  Load first, then m_rate/m_duration (WriteCFloat/ReadCFloat).
    void Save(void* writer);
    void Load(void* reader);

    //  0x6F48AF30 - vtable slot 15. CAgentRelation::ComputeChecksum(hash)
    //  via the base, then folds m_rate then m_duration into `*hash`.
    void ComputeChecksum(unsigned int* hash);

    //  0x6F48AFE0 - vtable slot 16. CAgentRelation::DumpDebugState(sink)
    //  via the base, then "[ReContMod]" and "\trte[%s] dur[%s]".
    void DumpDebugState(void* sink);

    //  ---- base-chain territory (CAgentRelation and below) - see
    //  Agent/agentrelation.h - plus this class's own added fields.
    char   m_baseReserved00[0x50];
    CFloat m_rate;       // +0x50
    CFloat m_duration;   // +0x54
};

}  // namespace NIpse

#endif  // CPORECONTINUOUSMOD_H
