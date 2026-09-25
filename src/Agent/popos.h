//============================================================================
//  NIpse::CPoPos (`??_7CPoPos@NIpse@@6B@`, 0x6F9524A4, 24 slots) - the base
//  of the small "PoPos*" position/relation sub-family this cluster's own
//  task handed down alongside NIpse::CPoPosBh (poposbh.h), NIpse::CPoPosCl
//  (poposcl.h) and NTempest::CLinkedList<NIpse::CPoPosCl> (poposcl.h) - a
//  follow-up to the earlier "cluster H" session's own NIpse::CPoSeparate
//  (Agent/poseparate.h), which that file's own header comment explicitly
//  left these four classes for later.
//
//  Slots 5-8, 11-14 and 17 are the shared no-ops every class in this whole
//  NTempest/NIpse chain inherits unchanged (SVtableNoOp/SVtableNoOpArg,
//  per the task's own cross-reference against `agent_worktrees/classes/`);
//  slot 18 is the already-established `CAgentRelation::OnRegistrationRetired`
//  (agentrelation.h, 0x6F4A69B0) - CPoPos never overrides it, nor does
//  CPoPosCl (CPoPosBh does - see poposbh.h). Not modelled again here.
//
//  **Nothing to do with the already-reconstructed Position/PositionTrack/
//  SmartPosition/ProjectilePosition family** (Pathfinding/positiontrack.h) -
//  same caution poseparate.h's own header already raises for CPoSeparate.
//  `CPoPos`'s own real vtable is `??_7CPoPos@NIpse@@6B@` (0x6F9524A4).
//
//  ---------------------------------------------------------------------
//  FLAT, offset-addressed - like Pathfinding/crlagent.h's own CRlAgent, NOT
//  real C++ inheritance from CRlProperty. Found the hard way this session:
//  CPoPos's own vtable slot 4 (Slot4) tail-calls the exact address
//  Agent/rlproperty.h names `CRlProperty::Slot4` against the SAME `this`,
//  which looks exactly like a base-class call - but `NIpse::CRlProperty`
//  carries NO data members of its own (rlproperty.h's own header comment:
//  "No real C++ layout is modelled here beyond what this call tree
//  touches" - gaps are left un-named, not reserved as padding), so
//  `class CPoPos : public CRlProperty` would place CPoPos's own +0x78-only
//  fields at the empty-base-optimised offset 0 instead of the shipped
//  +0x78 - a real, silent layout bug this session's own first Slot4 build
//  caught at 0.75/1.000 (`[esi+28]`/`[esi+32]` generated vs the dump's own
//  `[esi+148]`/`[esi+152]`) before it went anywhere. Every base-class-
//  shaped call below is instead the SAME `((CRlProperty*)this)->Method(...)`
//  reinterpret-cast convention Agent/rlproperty_apply.cpp's own
//  `CRlProperty::Apply` already uses one level up this chain to reach
//  `CAgentRelation`'s methods - it works regardless of inheritance because
//  CRlProperty's own methods only ever touch fixed byte offsets from
//  `this`, never a declared member of their own.
//
//  CPoPos's own added fields start at +0x78 (CRlProperty's own +0x00-+0x77
//  modelled here as opaque padding, exactly like crlagent.h's own
//  `m_baseReserved*` fields), all seven +0x78..+0x90 confirmed
//  CFloat-shaped by Save/Load's own identical WriteCFloat/ReadCFloat
//  treatment of every one of them regardless of how Apply happens to
//  initialise each (Apply/DumpDebugState below cross-reference the exact
//  per-field story):
//
//    +0x78  m_baseX   / +0x7C  m_baseY   - "[Pos]" tag's own "base x[%s]
//           y[%s]" (DumpDebugState); Apply stamps both with the same
//           fixed class-id constant (dword_6FAB7768) rather than deriving
//           them per-instance.
//    +0x80  m_velX    / +0x84  m_velY    - same dump line's "vel x[%s]
//           y[%s]" - zeroed by Apply.
//    +0x88  m_fieldD  ("maxv") / +0x8C m_fieldE ("ori") / +0x90 m_fieldF
//           ("rad") - DumpDebugState's own third line, "\tmaxv[%s]
//           ori[%s] rad[%s]". Zeroed by Apply alongside +0x78/+0x7C
//           (the shipped code writes all four with the same
//           `dword_6FAAE470` CFloat-zero bit pattern) EXCEPT +0x88, which
//           Apply resolves from the incoming `SHandleRegistrationRequest*`
//           (or a shared default when null) instead.
//    +0x94  m_syncChannelA / +0x98 m_syncChannelB - two
//           `CGridRegistration*`-shaped registration handles (spatialgrid.h,
//           already real); Slot4 tears both down through the already-
//           established `CGridRegistration::TeardownRegistration`
//           (0x6F4A00B0, no null guard at either call site - matches the
//           shipped code exactly). **Apply's own exact call establishing
//           these is left a THUNK this session** (not a full
//           reconstruction) - see Apply's own doc comment below for why.
//============================================================================
#ifndef POPOS_H
#define POPOS_H

#include "rlproperty.h"
#include "CFloat.h"
#include "agentbasechain_staticpools.h"

struct SHandleRegistrationRequest;
class CDataStoreScratch;
class CDataStore;
class CGridRegistration;

namespace NIpse {

class CPoPos
{
public:
    //  0x6F487680 - vtable slot 0. Literal "PoPos", no `this` read.
    const char* GetTypeName();

    //  0x6F487A10 - vtable slot 1. Same unlink-then-return-to-pool shape
    //  as every sibling in this chain, against this class's own static
    //  pool (CPoPos_GetStaticPool, agentbasechain_staticpools.h).
    void ReturnToStaticPool();

    //  0x6F487A50 - vtable slot 2, scalar deleting destructor. Same
    //  no-base-teardown restamp-to-CPresence triplet Agent/presence.h's
    //  own header comment documents (a further member of that same
    //  family, alongside CPoSeparate's/CRlProperty's own).
    void* ScalarDeletingDestructorTrivial(unsigned int flags);

    //  0x6F4A7570 - vtable slot 3, "Apply". Left a THUNK this session:
    //  the base `CRlProperty::Apply` call and the +0x78/+0x7C/+0x80/+0x84
    //  stamps are simple, but the tail resolves +0x94/+0x98 through
    //  `CTimeSync::GetCellGrid()`/`SField238Owner::GetField238()` feeding
    //  a shared registration helper (0x6F4A03F0) whose own local 9-dword
    //  staging buffer and exact stack-arg shape were not confidently
    //  re-derived from the disassembly alone this session - a wrong
    //  guess here is exactly the "correctness, not just shape" trap
    //  pipeline/CLAUDE.md warns about, so this is left a genuine
    //  `mov eax,<addr>/jmp eax` redirect (`THUNK` in agent_worktrees)
    //  rather than a confident-looking wrong reconstruction. CPoPosBh's
    //  and CPoPosCl's own Apply overrides (poposbh.h/poposcl.h) call this
    //  same address for their own base-call portion and are fully
    //  reconstructed independently of this gap.
    void Apply(SHandleRegistrationRequest* arg);

    //  0x6F4A7650 - vtable slot 4, "Slot4". Sets the 0x1000000 flags bit,
    //  detaches the base's own dependents
    //  (`((CRlProperty*)this)->DetachDependents()`), tears down both
    //  +0x94/+0x98 registration handles through
    //  `CGridRegistration::TeardownRegistration`, then tail-calls the
    //  base `((CRlProperty*)this)->Slot4(arg)`.
    void Slot4(int arg);

    //  0x6F4A76A0 - vtable slot 9, Save. Base `CRlProperty::Save` first,
    //  then this class's own seven added CFloats (+0x78 through +0x90, in
    //  offset order, via WriteCFloat) and finally the two registration
    //  handles at +0x94/+0x98 via WritePathRefEntry (passed BY VALUE, not
    //  by address - the handle itself, not a CFloat).
    void Save(CDataStoreScratch* store);

    //  0x6F4A7730 - vtable slot 10, Load. Save's exact mirror image
    //  (ReadCFloat / SPathRefReader::ReadEntry).
    void Load(CDataStore* store);

    //  0x6F487690 - vtable slot 15, ComputeChecksum. Folds the base
    //  `CRlProperty::ComputeChecksum` first, then this class's own seven
    //  added CFloats (+0x78 through +0x90 inclusive, in offset order)
    //  with the same per-byte `(b*0x7A2D)^b` fold this whole family
    //  shares - NOT +0x94/+0x98 (those are handles, not folded).
    void ComputeChecksum(unsigned int* hash);

    //  0x6F4878B0 - vtable slot 16, DumpDebugState. Dumps
    //  `CAgentRelation::DumpDebugState` DIRECTLY (agentrelation.h,
    //  0x6F4A62D0) - NOT through `CRlProperty::DumpDebugState`'s own
    //  wrapper (0x6F4874F0, which prints a "[Prop]" tag this class does
    //  not want) - then this object's own "[Pos]" tag and its seven
    //  position/velocity CFloats.
    void DumpDebugState(void* sink);

    //  0x6F4A7480 - vtable slot 19. THUNK (`popos_slots1920_thunk.cpp`):
    //  genuinely new to this class (not the shared base-chain no-ops), but
    //  its own body (a CFloat conversion through `sub_6F493910` stamped
    //  into +0x8C) reaches a small CFloat value-mod-threshold helper
    //  (sub_6F493910 -> sub_6F6EFE50) this THIRD follow-up session traced
    //  two levels deeper than either prior session - far enough to place
    //  it as a generalised sibling of Math/floatmodstep.cpp's own
    //  `CFloatModStep`/Math/cfloatwrappeddelta.cpp's own
    //  `CFloatWrappedDelta` (both already real), reusing the same already-
    //  real `CFloatReciprocal`/`CFloatFrac`/`operator*`/`operator+`/
    //  `operator-` quintet - but NOT far enough to confidently sign
    //  `sub_6F6EFE50`'s own two near-boundary correction branches (which
    //  of the two ways past each boundary the shipped code goes). See
    //  `docs/targets/NTempestNIpse_clusterH_remainder.md`'s own "further
    //  follow-up (round 2)" section for the full trace and why this
    //  stays a THUNK rather than a guess. `CPoPosBh` inherits it
    //  unchanged; `CPoPosCl` overrides it with its own real body,
    //  `poposcl_slots192022.cpp`, which tail-calls this.
    void Slot19(unsigned int arg);

    //  0x6F4A74A0 - vtable slot 20. Real now (`popos_slot20.cpp`) - it
    //  turned out to have NOTHING to do with Slot19's own still-unresolved
    //  web above (a completely different, fully-closed leaf pair instead:
    //  `SCFloatVec2::AngleFromLength`/`::ComputeAngle`,
    //  `Math/cfloatvec2angle.cpp`, an acos-based 2D-vector-to-angle helper
    //  reused nowhere else in this class's own call tree, applied to the
    //  velocity pair at +0x80/+0x84 and stamped into +0x8C when the
    //  velocity's own squared length clears a small epsilon). DIFFERS -
    //  one stack-slot-count difference from the shipped body, otherwise
    //  every instruction present and in order; see its own
    //  `agent_worktrees` reason.
    void Slot20();

    //  0x6F4879B0 - vtable slot 21. Genuine one-instruction no-op at its
    //  own address (not the shared `SVtableNoOp`).
    void Slot21(unsigned int);

    //  0x6F4879C0 - vtable slot 22. Same shape, no arguments.
    void Slot22();

    char   m_base00_78[0x78];   // +0x00-+0x77, CRlProperty's own territory
                                  // (unmodeled - see this file's own header
                                  // comment for why this stays flat).
    CFloat m_baseX;          // +0x78 - "base x" in DumpDebugState
    CFloat m_baseY;          // +0x7C - "base y"
    CFloat m_velX;           // +0x80 - "vel x"
    CFloat m_velY;           // +0x84 - "vel y"
    CFloat m_fieldD;         // +0x88 - not printed by anything this
                              //         session reconstructed
    CFloat m_fieldE;         // +0x8C - same caveat
    CFloat m_fieldF;         // +0x90 - same caveat
    CGridRegistration* m_syncChannelA;   // +0x94
    CGridRegistration* m_syncChannelB;   // +0x98
};

//  0x6F4A6D60 - CPoPos's own static-pool getter (matches every sibling in
//  this chain's own one-instruction leaf shape); declared globally in
//  agentbasechain_staticpools.h alongside every sibling's own getter, not
//  redeclared here.
//
//  0x6F4A71F0 - Apply's own "resolve or default" helper (same three-way
//  shape as poposbh.h's own 0x6F4AAB70/0x6F4AABB0) is left TODO,
//  unreconstructed, alongside Apply itself - see Apply's own doc comment
//  above.

}  // namespace NIpse

#endif  // POPOS_H
