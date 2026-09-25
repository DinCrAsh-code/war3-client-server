//============================================================================
//  NIpse::CPoPoVelocityMod (`??_7CPoPoVelocityMod@NIpse@@6B@`, 0x6F952BF4,
//  20 slots) - the sibling of NIpse::CPoPoInterfLis (cpopointerflis.h,
//  same header's own file comment for the shared shape/hierarchy), also
//  branching directly off NIpse::CPrRelation/CAgentRelation
//  (Agent/agentrelation.h). RTTI hierarchy confirmed
//  (agent_worktrees/classes/0x6F952BF4.json):
//  "CPoPoVelocityMod: NIpse::CPrRelation, NTempest::CPresenceTagged,
//  NTempest::CPresence, NTempest::CLinkedNode<NTempest::CPresence>;".
//
//  Slots 17/18/19 are the same shared-no-op / inherited-`CAgentRelation::
//  OnRegistrationRetired` / confirmed-misattributed trio
//  cpopointerflis.h's own file header documents in detail for its sibling
//  (identical addresses: 0x6F47FFE0/0x6F4A69B0/0x6F506EA0) - not repeated
//  here. Unlike its sibling, this class has NO out-of-scope work: all ten
//  of its own slots (0-4, 8, 9, 10, 15, 16) are fully reconstructed - it
//  carries no array/`CMemBlock` member, just three plain `CFloat` fields.
//
//  Fields (confirmed from Apply's/Save's/Load's/ComputeChecksum's/
//  DumpDebugState's own raw_asm, all consistent):
//    m_velocityX (CFloat, +0x50) - from `arg`'s own +0x10/+0x14 pair
//      (PoPoVelocityModQueryVelocity, a hidden-return-pointer leaf
//      returning both dwords of a 2-CFloat pair at once).
//    m_velocityY (CFloat, +0x54)
//    m_duration  (CFloat, +0x58) - from `arg`'s own +0x14 field
//      (PoPoVelocityModQueryDuration), default g_CFloatZero.
//
//  Slot 8 (NotifyEndpointAIfTagged) reacts to '^rel'-tagged ('^beg'/'^end')
//  context messages by publishing this object's own velocity (negated on
//  '^end') to endpoint B's own CPathTrace
//  (GetField0x40_6F4A7840/CPathTrace::PublishVelocityDelta, both already
//  reconstructed - Widget/positionsinksettarget.cpp/
//  Pathfinding/pathtracepublishvelocity.cpp), and on '^beg' additionally
//  decides whether m_duration should be treated as "still running"
//  (UpdateDurationRegistration, this class's own new leaf, 0x6F4A83F0).
//============================================================================
#ifndef CPOPOVELOCITYMOD_H
#define CPOPOVELOCITYMOD_H

#include "CFloat.h"

struct SHandleRegistrationRequest;

namespace NIpse {

class CPoPoVelocityMod
{
public:
    //  0x6F48B720 - vtable slot 0. Literal string "PoPoVelocityMod".
    const char* GetTypeName();

    //  0x6F48B8E0 - vtable slot 1. Own static pool,
    //  CPoPoVelocityMod_GetStaticPool.
    void ReturnToStaticPool();

    //  0x6F48B920 - vtable slot 2, scalar deleting destructor. The plain
    //  restamp-to-`CPresence`-vftable triplet shape (Agent/presence.h's own
    //  file header) - unlike its sibling CPoPoInterfLis, this class has no
    //  member of its own that needs a real destructor call first.
    void* ScalarDeletingDestructorTrivial(unsigned int flags);

    //  0x6F4A7D20 - vtable slot 3. ResetEventBinding(arg) via the base,
    //  then m_velocityX/m_velocityY from `arg`'s own +0x10/+0x14 pair and
    //  m_duration from `arg`'s own +0x14... (see .cpp - the two helper
    //  leaves read different, adjacent `arg` offsets), then
    //  flags |= 0x40044.
    void Apply(SHandleRegistrationRequest* arg);

    //  0x6F4A6C90 - vtable slot 4. A bare, single `jmp` straight into
    //  CAgentRelation::Teardown - MSVC's own sibling-call optimisation
    //  (docs/msvc-vc8-idioms.md), not a naked thunk: writing this as an
    //  ordinary `((CAgentRelation*)this)->Teardown(arg);` call reproduces
    //  the tail jmp exactly.
    void Slot4(int arg);

    //  0x6F4A8500 - vtable slot 8. REAL override. See cpopovelocitymod_notify.cpp.
    void* NotifyEndpointAIfTagged(const void* arg);

    //  0x6F4A83F0 - UpdateDurationRegistration, Slot8's own '^beg' tail
    //  call. Not a vtable slot. Compares m_duration (+0x58) against a fixed
    //  global (g_unk6FAAE624) and either marks kRelWantSample + takes an
    //  unconditional sampled registration, or clears it and takes a
    //  registration timed directly off m_duration's own address - see
    //  cpopovelocitymod_notify.cpp for the exact FPU comparison polarity
    //  this reproduces.
    void UpdateDurationRegistration();

    //  0x6F4A7850 / 0x6F4A7890 - vtable slots 9/10. CAgentRelation::Save/
    //  Load first, then m_velocityX, m_velocityY, m_duration in that order
    //  (WriteCFloat/ReadCFloat x3).
    void Save(void* writer);
    void Load(void* reader);

    //  0x6F48B730 - vtable slot 15. CAgentRelation::ComputeChecksum(hash)
    //  via the base, then folds m_velocityX, m_velocityY, m_duration in
    //  that order into `*hash`.
    void ComputeChecksum(unsigned int* hash);

    //  0x6F48B820 - vtable slot 16. CAgentRelation::DumpDebugState(sink)
    //  via the base, then "[PoVelMod]" and
    //  "\tvel x[%s] y[%s] dur[%s]" of m_velocityX/m_velocityY/m_duration.
    void DumpDebugState(void* sink);

    //  ---- base-chain territory (CAgentRelation and below) - see
    //  Agent/agentrelation.h - plus this class's own added fields.
    char   m_baseReserved00[0x50];
    CFloat m_velocityX;   // +0x50
    CFloat m_velocityY;   // +0x54
    CFloat m_duration;    // +0x58
};

}  // namespace NIpse

#endif  // CPOPOVELOCITYMOD_H
