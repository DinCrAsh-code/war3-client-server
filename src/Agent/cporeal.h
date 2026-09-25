//============================================================================
//  NIpse::CPoReal (`??_7CPoReal@NIpse@@6B@`, 0x6F952DEC, 19 slots) - cluster
//  F's follow-up session, 2026-09-08 (see cpoflag.h's own file header for
//  the shared "plain subset" notes - no slot-8 override, idx4 is the same
//  shared 0x6F4975C0 a different concurrent session already reconstructed).
//
//  Layout: extends `NIpse::CRlProperty` (Agent/rlproperty.h, offsets
//  0x00-0x74) with a clamped value at +0x78, a field at +0x7C always reset
//  to `g_CFloatZero`'s own bits by Apply, and a two-dword clamp range at
//  +0x80/+0x84 (low/high bound).
//
//  own new slots (this session's batch):
//    idx0  0x6F48C400  GetTypeName            -> "PoReal"
//    idx1  0x6F48C660  ReturnToStaticPool      (own pool, CPoReal_GetStaticPool)
//    idx2  0x6F48C6B0  ScalarDeletingDestructorTrivial (restamp-to-CPresence triplet)
//    idx3  0x6F4A93F0  Apply   - CRlProperty::Apply(arg), refreshes the
//                      +0x80/+0x84 clamp range, then stores
//                      clamp(candidate, lo, hi) at +0x78 and 0 at +0x7C
//    idx9  0x6F4A9470  Save    - CRlProperty::Save(writer), then writes
//                      +0x78/+0x7C/+0x80/+0x84 (four CFloats)
//    idx10 0x6F4A94C0  Load    - CRlProperty::Load(reader), mirror of Save
//    idx15 0x6F48C410  ComputeChecksum - CRlProperty::ComputeChecksum(hash),
//                      then folds all four fields (+0x78/+0x7C/+0x80/+0x84)
//    idx16 0x6F48C550  DumpDebugState  - CRlProperty::DumpDebugState(sink),
//                      then "[Real]" / "\tbas[%s] chn[%s]" / "\tbnds l[%s] h[%s]"
//============================================================================
#ifndef CPOREAL_H
#define CPOREAL_H

#include "CFloat.h"

struct SHandleRegistrationRequest;

namespace NIpse {

//  Raw {lo, hi} bit-pattern pair - see cporeal_queryrange.cpp.
struct SPoRealRange
{
    unsigned int m_lo;
    unsigned int m_hi;
};

class CPoReal
{
public:
    //  0x6F48C400 - vtable slot 0. Literal "PoReal" (aPoreal).
    const char* GetTypeName();

    //  0x6F48C660 - vtable slot 1. Own static pool, CPoReal_GetStaticPool.
    void ReturnToStaticPool();

    //  0x6F48C6B0 - vtable slot 2, scalar deleting destructor.
    void* ScalarDeletingDestructorTrivial(unsigned int flags);

    //  0x6F4A93F0 - vtable slot 3. Written naked - see cporeal_apply.cpp
    //  for the full clamp derivation this transcribes exactly.
    void Apply(SHandleRegistrationRequest* arg);

    //  0x6F4A9470 / 0x6F4A94C0 - vtable slots 9/10. CRlProperty::Save/Load
    //  first, then this class's own four CFloats (+0x78/+0x7C/+0x80/+0x84,
    //  via WriteCFloat/ReadCFloat, in that offset order).
    void Save(void* writer);
    void Load(void* reader);

    //  0x6F48C410 - vtable slot 15. CRlProperty::ComputeChecksum(hash) via
    //  the base, then folds all four of this class's own fields (in the
    //  same +0x78/+0x7C/+0x80/+0x84 order) into `*hash`.
    void ComputeChecksum(unsigned int* hash);

    //  0x6F48C550 - vtable slot 16. CRlProperty::DumpDebugState(sink) via
    //  the base (duplicated inline - see .cpp), then "[Real]",
    //  "\tbas[%s] chn[%s]" (+0x7C, +0x78) and "\tbnds l[%s] h[%s]"
    //  (+0x84, +0x80).
    void DumpDebugState(void* sink);

    //  ---- base-chain territory (CRlProperty and below) - see
    //  Agent/rlproperty.h - plus this class's own added fields.
    char   m_baseReserved00[0x78];
    CFloat m_value;     // +0x78 - clamp(candidate, lo, hi)
    CFloat m_chn;       // +0x7C - always reset to g_CFloatZero by Apply
    CFloat m_rangeLo;   // +0x80
    CFloat m_rangeHi;   // +0x84
};

}  // namespace NIpse

#endif  // CPOREAL_H
