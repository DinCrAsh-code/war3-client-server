//============================================================================
//  NIpse::CPoFgPulseMod (`??_7CPoFgPulseMod@NIpse@@6B@`, 0x6F952C7C, 19
//  slots) - cluster F's follow-up session, 2026-09-08. Listener/modifier
//  subset (real slot-8 override), reaches CAgentRelation directly like
//  Agent/cporecontinuousmod.h's own sibling.
//
//  Layout: extends `NIpse::CAgentRelation` (offsets 0x00-0x4F) with five
//  fields of its own, named from DumpDebugState's own printed labels
//  (cpofgpulsemod_dump.cpp) rather than guessed: m_mask (+0x50), m_delta
//  (+0x54), a CFloat m_frequency (+0x58), m_duration (+0x5C), and
//  m_playfield (+0x60, always zeroed by Apply).
//
//  own new slots (this session's batch):
//    idx0  0x6F48BA00  GetTypeName            -> "PoFgPulseMod"
//    idx1  0x6F48BC50  ReturnToStaticPool      (own pool, CPoFgPulseMod_GetStaticPool)
//    idx2  0x6F48BC90  ScalarDeletingDestructorTrivial (restamp-to-CPresence triplet)
//    idx3  0x6F4A8B80  Apply   - ResetEventBinding(arg), four fields from
//                      `arg` (offsets 0x0C/0x10/0x14/0x18), m_playfield=0,
//                      flags |= 0x50404
//    idx4  0x6F4A88E0  Slot4   - tail-calls CAgentRelation::Teardown(arg)
//    idx8  0x6F4A8D60  NotifyEndpointAIfTagged (naked, `'^rel'`-tagged,
//                      "^beg"/"^sig" reacting - see .cpp)
//    idx9  0x6F4A8BD0  Save    - CAgentRelation::Save(writer), then writes
//                      all five fields
//    idx10 0x6F4A8C20  Load    - CAgentRelation::Load(reader), mirror
//    idx15 0x6F48BA10  ComputeChecksum - folds all five fields
//    idx16 0x6F48BB90  DumpDebugState  - "[FgPulseMod]" /
//                      "\tmsk[0x%X] dlt[0x%X]" (m_mask, m_delta) /
//                      "\tfrq[%s] dur[%u] plf[%u]" (m_frequency, m_duration,
//                      m_playfield)
//============================================================================
#ifndef CPOFGPULSEMOD_H
#define CPOFGPULSEMOD_H

#include "CFloat.h"

struct SHandleRegistrationRequest;

namespace NIpse {

class CPoFgPulseMod
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

    //  0x6F4A8B10 - `this`-implicit leaf, returns +0x40 (endpoint B, same
    //  shape as Agent/cpofglis.h's own GetEndpointBRaw at a different
    //  address - docs/msvc-vc8-idioms.md's own "Four identical functions
    //  at four addresses" idiom, another instance). Named GetEndpointBRaw,
    //  not GetPlayfieldRaw - it reads the endpoint-B field, unrelated to
    //  this class's own m_playfield below (a same-sounding coincidence).
    void* GetEndpointBRaw();

    char         m_baseReserved00[0x50];
    unsigned int m_mask;        // +0x50 (dword, -1 default off `arg`+0x0C)
    unsigned int m_delta;       // +0x54 (dword, 0 default off `arg`+0x10)
    CFloat       m_frequency;   // +0x58 (CFloatZero default off `arg`+0x14)
    unsigned int m_duration;    // +0x5C (dword, 0 default off `arg`+0x18)
    unsigned int m_playfield;   // +0x60 - zeroed by Apply
};

}  // namespace NIpse

#endif  // CPOFGPULSEMOD_H
