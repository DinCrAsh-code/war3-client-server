//============================================================================
//  NIpse::CPoFgLis (`??_7CPoFgLis@NIpse@@6B@`, 0x6F952D24, 19 slots) - a
//  second sibling of the "Po-modifier/listener" family branching off
//  NIpse::CPrRelation/CAgentRelation (Agent/agentrelation.h) - see
//  docs/targets/NTempestNIpse_PoModifierListener_ClusterF.md for the full
//  9-class family.
//
//  Unlike NIpse::CPoLkLis (Agent/cpolklis.h, the family's one exception),
//  this class DOES add its own field - a single mask dword at +0x50 - and
//  DOES override slots 15/16 (ComputeChecksum/DumpDebugState) for real,
//  folding that field into both. Kept flat/offset-addressed for the same
//  reason CPoLkLis is: the layout is hand-verified per field, not derived
//  from a real base sub-object.
//
//  own new slots (this session's batch):
//    idx0  0x6F48BD40  GetTypeName            -> "PoFgLis"
//    idx1  0x6F48BE40  ReturnToStaticPool      (own pool, CPoFgLis_GetStaticPool)
//    idx2  0x6F48BE80  ScalarDeletingDestructorTrivial (restamp-to-CPresence triplet)
//    idx3  0x6F4A8CD0  Slot3   - ResetEventBinding(arg), m_mask = query(arg), flags |= 0x400
//    idx4  0x6F4A88F0  Slot4   - tail-calls CAgentRelation::Teardown(arg) directly
//    idx8  0x6F4A8E10  NotifyEndpointAIfTagged - REAL override (this family's
//                      "*Lis"/"*Mod" subset), filters on endpointB's own
//                      +0x78 field XORed against arg's own +0x10 field,
//                      masked by m_mask, before firing
//    idx9  0x6F4A8D00  Save    - CAgentRelation::Save(writer), then writes m_mask
//    idx10 0x6F4A8D20  Load    - CAgentRelation::Load(reader), then reads m_mask
//    idx15 0x6F48BD50  ComputeChecksum - CAgentRelation::ComputeChecksum(hash),
//                      then folds m_mask's own four bytes into *hash
//    idx16 0x6F48BDB0  DumpDebugState  - CAgentRelation::DumpDebugState(sink),
//                      then "[FgLis]" / "\tmsk[0x%X]" of m_mask
//============================================================================
#ifndef CPOFGLIS_H
#define CPOFGLIS_H

struct SHandleRegistrationRequest;

namespace NIpse {

class CPoFgLis
{
public:
    //  0x6F48BD40 - vtable slot 0. Literal string "PoFgLis" (aPofglis).
    const char* GetTypeName();

    //  0x6F48BE40 - vtable slot 1. Same shape as CPoLkLis::ReturnToStaticPool
    //  (Agent/cpolklis.h), against this class's own static pool
    //  (CPoFgLis_GetStaticPool, Agent/agentbasechain_staticpools.h/.cpp).
    void ReturnToStaticPool();

    //  0x6F48BE80 - vtable slot 2, scalar deleting destructor. Same
    //  restamp-to-CPresence-vftable triplet shape as CPoLkLis's own
    //  (Agent/presence.h's own file header).
    void* ScalarDeletingDestructorTrivial(unsigned int flags);

    //  0x6F4A8CD0 - vtable slot 3. ResetEventBinding(arg) through the base
    //  (Agent/agentrelation.h), then stores a queried mask value from `arg`
    //  into this class's own +0x50 field (SPoFgLisMaskSource::QueryMask,
    //  below), then sets flags bit 0x400 - same bit CPoLkLis::Slot3 sets.
    void Slot3(SHandleRegistrationRequest* arg);

    //  0x6F4A88F0 - vtable slot 4. Single-`jmp` tail call into
    //  CAgentRelation::Teardown, identical shape to CPoLkLis::Slot4.
    void Slot4(int arg);

    //  0x6F4A8E10 - vtable slot 8. REAL override: filters `arg` the same
    //  way CPoLkLis's own slot 8 does (two magic-constant field checks),
    //  additionally XORs endpoint B's own +0x78 field against `arg`'s own
    //  +0x10 field and requires the result to overlap this object's own
    //  m_mask before firing the "^lis" event context to endpoint A. Naked -
    //  see cpofglis_notify.cpp for why.
    void* NotifyEndpointAIfTagged(const void* arg);

    //  0x6F4A8D00 - vtable slot 9. CAgentRelation::Save(writer), then writes
    //  this class's own m_mask.
    void Save(void* writer);

    //  0x6F4A8D20 - vtable slot 10. CAgentRelation::Load(reader), then reads
    //  this class's own m_mask.
    void Load(void* reader);

    //  0x6F48BD50 - vtable slot 15. CAgentRelation::ComputeChecksum(hash) via
    //  the base, then folds m_mask's own four bytes into `*hash` one byte at
    //  a time through a small multiplicative mix (see cpofglis_dump.cpp for
    //  the exact spelling this reconstructs byte-for-byte). A REAL override,
    //  unlike CPoLkLis, which inherits this slot unchanged.
    void ComputeChecksum(unsigned int* hash);

    //  0x6F48BDB0 - vtable slot 16. CAgentRelation::DumpDebugState(sink) via
    //  the base, then this object's own "[FgLis]" tag and its m_mask,
    //  formatted "\tmsk[0x%X]". A REAL override, unlike CPoLkLis.
    void DumpDebugState(void* sink);

    //  0x6F4A8CC0 - one-instruction leaf, own TU: `this`'s own endpoint B
    //  (CAgentRelation::m_endpointB, Agent/agentrelation.h, +0x40). Same
    //  shape as the already-reconstructed CDrivenRelation::GetTrace
    //  (0x6F4A9A50, agentrelation.h) - a second, separately-compiled copy of
    //  the identical one-instruction accessor at a different address, the
    //  "/OPT:ICF off" idiom docs/msvc-vc8-idioms.md's own "Four identical
    //  functions at four addresses is normal here" section documents.
    void* GetEndpointBRaw();

    //  ---- base-chain territory (CAgentRelation and below) - not modeled
    //  here, see Agent/agentrelation.h - plus this class's own added field.
    char         m_baseReserved00[0x50];
    unsigned int m_mask;   // +0x50
};

}  // namespace NIpse

//  0x6F4A8C70 - NIpse::CPoFgLis's own static pool: g_pTimeSync + 0x8B8.
//  Global scope, matching every other getter in
//  Agent/agentbasechain_staticpools.h (see that file's own precedent, and
//  Agent/cpolklis.h's own file header note on why the C++ name in
//  tools/funcmap.py must match this exactly, unqualified).
void* CPoFgLis_GetStaticPool();

//  0x6F4A8C80 - a genuinely `this`-implicit, `ecx`-only, `retn 0` leaf: given
//  `this` (a `SHandleRegistrationRequest*`, opaque here - see
//  Agent/agentrelation.h's own forward declaration), returns its own +0x0C
//  field, or -1 when `this` is null. A free function cannot be spelled
//  `__thiscall` in this compiler (Agent/agentbaseabsnotify.cpp's own note,
//  also cited by Misc/nullsub.cpp's `SVtableNoOpArg`), so - exactly like
//  that file's own `SAgentBaseAbsNotifySubject` - this is written as an
//  ordinary non-virtual method of a plain struct instead, called through a
//  pointer that may be null (legal here because the body checks before
//  dereferencing). Not folded into Pathfinding/handletable.h's own
//  `SHandleRegistrationRequest` definition: that header currently collides
//  with a different, incompatible `SHandleTable` a concurrent session has
//  added to Game/game.h (a live build defect this session did not introduce
//  and is out of scope to fix), so this stays a raw offset read against a
//  small standalone struct rather than pulling that header in. Own
//  translation unit (cpofglis_querymask.cpp), deliberately: called from
//  CPoFgLis::Slot3's own TU, and putting the two together let /Ob2 inline
//  this leaf away - the real dump shows the call staying out of line.
struct SPoFgLisMaskSource
{
    int QueryMask();
};

#endif  // CPOFGLIS_H
