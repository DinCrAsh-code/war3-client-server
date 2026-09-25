//============================================================================
//  NIpse::CPoRePulseMod (`??_7CPoRePulseMod@NIpse@@6B@`, 0x6F952AD4, 19
//  slots) - cluster F's follow-up session, 2026-09-08. Listener/modifier
//  subset (real slot-8 override), reaches CAgentRelation directly.
//
//  Layout: extends `NIpse::CAgentRelation` (offsets 0x00-0x4F) with four
//  fields: m_interval (+0x50, CFloat), m_frequency (+0x54, CFloat), m_opt
//  (+0x58, dword), m_pulseCount (+0x5C, dword, always zeroed by Apply).
//
//  own new slots (this session's batch):
//    idx0  0x6F48B1A0  GetTypeName            -> "PoRePulseMod"
//    idx1  0x6F48B3B0  ReturnToStaticPool      (own pool, CPoRePulseMod_GetStaticPool)
//    idx2  0x6F48B3F0  ScalarDeletingDestructorTrivial (restamp-to-CPresence triplet)
//    idx3  0x6F4A9710  Apply   - ResetEventBinding(arg), m_interval/
//                      m_frequency/m_opt from `arg` (0x0C/0x10/0x14),
//                      m_pulseCount=0, flags |= 0x50404
//    idx4  0x6F4A8F40  Slot4   - tail-calls CAgentRelation::Teardown(arg)
//    idx8  0x6F4A9760  NotifyEndpointAIfTagged (naked, `'^rel'`-tagged,
//                      "^beg"/"^sig" reacting - see .cpp)
//    idx9  0x6F4A97C0  Save    - CAgentRelation::Save(writer), then writes
//                      all four fields
//    idx10 0x6F4A9810  Load    - CAgentRelation::Load(reader), mirror
//    idx15 0x6F48B1B0  ComputeChecksum - folds all four fields
//    idx16 0x6F48B2F0  DumpDebugState  - "[RePulsMod]" /
//                      "\tint[%s] frq[%s]" / "\tdur[%10u] plf[%10u]"
//============================================================================
#ifndef CPOREPULSEMOD_H
#define CPOREPULSEMOD_H

#include "CFloat.h"

struct SHandleRegistrationRequest;

namespace NIpse {

class CPoRePulseMod
{
public:
    const char* GetTypeName();
    void ReturnToStaticPool();
    void* ScalarDeletingDestructorTrivial(unsigned int flags);
    void Apply(SHandleRegistrationRequest* arg);
    void Slot4(int arg);
    void* NotifyEndpointAIfTagged(const void* arg);
    void Save(void* writer);
    void Load(void* reader);
    void ComputeChecksum(unsigned int* hash);
    void DumpDebugState(void* sink);

    char   m_baseReserved00[0x50];
    CFloat       m_interval;    // +0x50 (CFloatZero default off `arg`+0x0C)
    CFloat       m_frequency;   // +0x54 (CFloatZero default off `arg`+0x10)
    unsigned int m_opt;         // +0x58 (dword, 0 default off `arg`+0x14)
    unsigned int m_pulseCount;  // +0x5C - zeroed by Apply
};

}  // namespace NIpse

#endif  // CPOREPULSEMOD_H
