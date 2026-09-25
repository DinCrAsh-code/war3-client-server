//============================================================================
//  CMissile - `??_7CMissile@@6B@` (0x6F89147C, 50 slots).  RTTI hierarchy
//  (agent_worktrees/classes/0x6F89147C.json, Class Informer import):
//
//      CMissile -> CBullet -> CBulletBase -> CWar3Image -> CAgentWar3
//               -> CAgent -> CObserver -> TRefCnt
//
//  CMissile adds no new vtable slots past CBullet's own 50 - it overrides
//  thirteen of them (slots 1, 7, 10, 13, 17, 18, 19, 22, 42, 45, 46, 47,
//  49); the other 37 pass straight through unchanged, CBullet's (or an
//  ancestor's) responsibility, not reconstructed again here.
//
//  Layout, from the base-construction chain fully inlined into CMissile's
//  own constructor (0x6F0A4210 - the only address in this dump's own
//  reachable closure that stamps `??_7CMissile@@6B@`, following exactly
//  the same "collapsing inline chain" shape CBulletBase's and CBullet's own
//  headers already document one and two levels down):
//
//      +0x90  m_floatD, a plain FloatMini (Math/floatmini.h) - confirmed by
//             DumpState below calling FloatMini::DumpTo (0x6F4770E0, the
//             same address CBulletBase's/CBullet's own FloatMinis use) on
//             it.
//      +0x98  m_floatE, a second plain FloatMini, same confirmation.
//      +0xA0  m_fieldA0Handle / m_fieldA4TypeTag (+0xA4) - a nullable
//             {handle, typeTag} pair, the exact shape itemhandleresolve.h's
//             `SOptionalHandleRef` documents and the sentinel {-1,-1} the
//             constructor stamps confirms.  This is the same "+0xA0 slot"
//             missile_thunderbolt.h already names and operates on by
//             punning `this` at this offset (`SHandleRefAt0xA0Owner`,
//             `CMissileThunderBoltRefs::ClearFieldA0Ref`/`SetFieldA0Ref`) -
//             declared here for real now that CMissile's own field layout
//             is being reconstructed, rather than only ever reached
//             through an offset pun.  DumpState dumps it through
//             `SRefMiniValue::DumpTo` (0x6F478550, Storm/textsink.h) -
//             the same dumper CBulletBase's own `m_targetHandle` pair uses
//             for its "+0x2C" ref (bulletbase_core.cpp) - not through
//             `SOptionalHandleRef`'s own type, which has no `DumpTo` of its
//             own; two names for the identical two-dword shape, matched
//             the same way bulletbase.h's own `m_targetHandle` field is.
//      +0xA8  m_reservedA8 - a plain dword, zeroed by the constructor,
//             written (not read) in SaveFlags via `WriteDwordAlt2`, and
//             touched by nothing else in this class's own reachable
//             closure - left a reserved gap rather than guessed at, same
//             convention bulletbase.h's own `m_reserved34` uses.
//
//  Size: 0xAC (0x90 + 0x08 + 0x08 + 0x08 + 0x04).
//============================================================================
#ifndef MISSILE_H
#define MISSILE_H

#include "bullet.h"

class CMissile : public CBullet
{
public:
    //====================================================================
    //  CMissile's vtable contribution.  Declaration order *is* vtable
    //  order, sorted by slot index; see docs/targets/CMissile.md for the
    //  address table.  All thirteen override one of CBullet's own 50 -
    //  CMissile adds no new slots of its own.
    //====================================================================
    //  slot 1 / +0x04 (0x6F0A4410) - the scalar-deleting destructor.  No
    //  separate non-scalar ~CMissile survives: the real destructor body
    //  (destruct m_floatD/m_floatE - trivial, elided, same collapsing shape
    //  bullet.h's own m_floatMini already documents; restamp
    //  m_projectilePosition down to its own base TRefCnt vtable; tail-call
    //  CBulletBase::~CBulletBase() *directly*, skipping CBullet's own
    //  vtable-slot-1 body the same way CBullet's own DeleteSelf skips
    //  CBulletBase's) is fully inlined here - the outermost level nothing
    //  further derived inlines it into in this dump's own closure.  See
    //  missile_ctordtor.cpp.
    virtual CMissile* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F2BB740) - a bare constant, same shape as every
    //  other level of this family.
    virtual unsigned int GetAgileTypeId() const;
    //  slot 10 / +0x28 (0x6F0A42A0) - a direct (non-virtual) call to the
    //  shared CAgent::GetRecordVersion leaf (agent.h, "always 0"), plus
    //  fourteen at this level - skips CBullet's own +11 override entirely,
    //  same "genuinely different additive constant reached by a genuinely
    //  different call" shape bullet.h's own slot-10 override already
    //  documents one level down.
    virtual int GetRecordVersion(int context);
    //  slot 13 / +0x34 (0x6F2BEC60) - if the +0xA0 ref resolves,
    //  unregisters this object as its observer
    //  (CMissileThunderBoltRefs::SetObserverRegistration(this, 0)), then
    //  tail-calls CBulletBase::Deactivate() directly (not virtually) -
    //  layers one extra step on top of the inherited body, the same
    //  "direct call on m_projectilePosition then tail-jump into the base"
    //  shape bullet.h's own Method_0xA8 (slot 42) already uses.  See
    //  missile_core.cpp.
    virtual void Deactivate();
    //  slot 17 / +0x44 (0x6F0A4330) - CBulletBase::DumpState (the direct,
    //  non-virtual base call - *not* CBullet::DumpState, so this level
    //  re-does CBullet's own m_floatMini dump inline rather than chaining
    //  through it), then m_floatMini/m_fieldA0.../m_floatD/m_floatE, in
    //  that field order.  See missile_dumpflags.cpp.
    virtual void DumpState(void* sink);
    //  slot 18 / +0x48 (0x6F0A42B0) - same base-call shape as DumpState:
    //  CBulletBase::SaveFlags directly, then this class's own m_floatMini
    //  through WriteCFloat, m_fieldA0Handle/m_fieldA4TypeTag/m_reservedA8
    //  through WriteDword/WriteDword/WriteDwordAlt2, then m_floatD/m_floatE
    //  through WriteCFloat.  See missile_dumpflags.cpp.
    virtual void SaveFlags(class CDataStoreScratch* store);
    //  slot 19 / +0x4C (0x6F0A4380) - the LoadFlags-slot counterpart:
    //  CBulletBase::LoadFlags directly, then m_floatMini/m_floatD/m_floatE
    //  each through ReadCFloat + their own vtable-slot-0 "assign and
    //  notify".  Notably does **not** read back m_fieldA0Handle/
    //  m_fieldA4TypeTag/m_reservedA8 at all - a real asymmetry with
    //  SaveFlags above, not a modelling gap (confirmed against the raw
    //  disassembly: no load of +0xA0/+0xA4/+0xA8 anywhere in this
    //  function).  See missile_dumpflags.cpp.
    virtual void LoadFlags(class CDataStore* store);
    //  slot 22 / +0x58 (0x6F2BB6F0) - "CMissile", this level's own
    //  GetClassName answer.
    virtual const char* GetClassName();
    //  slot 42 / +0xA8 (0x6F2BD010) - same shape as slot 13 (Deactivate)
    //  above: if the +0xA0 ref resolves, unregisters this object as its
    //  observer, then tail-calls CBullet::Method_0xA8() directly (not
    //  virtually).  See missile_core.cpp.
    virtual void Method_0xA8();
    //  slot 45 / +0xB4 (0x6F2C42D0) - was mis-filed under a helper
    //  organisational struct as `CMissileThunderBoltRefs::OnArrival`
    //  before this session (missile_thunderbolt.h/missile_arrival.cpp) -
    //  a name that pre-dated identifying which class actually owns this
    //  vtable slot.  It genuinely *is* reached "through the missile's own
    //  vtable slot 0xB4" (that file's own header already said so), which
    //  makes it CMissile's own override of CBulletBase::InvokeSlot0xBC
    //  (the tail-jump-to-slot-0xBC alias every level from CBulletBase
    //  through CBullet left untouched, see bulletbase.h) - not a method of
    //  a type CMissile merely happens to use.  Renamed here to
    //  `CMissile::OnArrival`, same verb (it is genuinely the arrival/impact
    //  handler), correctly qualified.  See missile_arrival.cpp for the
    //  full body and docs/targets/cmissilethunderbolt_impact_pipeline.md
    //  for the wider pipeline this frame belongs to.
    virtual void OnArrival();
    //  slot 46 / +0xB8 (0x6F2C1B80) - overrides CBullet's inherited
    //  CBulletBase::OnTargetRefNotify (0x6F2BE7E0, still the address
    //  CBullet's own slot 46 carries unchanged): calls it directly first,
    //  then checks whether the just-notified reference is the *same*
    //  object as this missile's own +0xA0 ref and, if a deeper
    //  reconfirmation (0x6F2BECE0, out of this session's depth budget)
    //  says it no longer holds, clears it.  Best-effort - see
    //  missile_targetrefnotify.cpp.
    virtual void OnTargetRefNotify(const SBulletDispatchMsg* msg);
    //  slot 47 / +0xBC (0x6F2BECB0) - overrides the `nullsub_1286` empty
    //  body every level from CBulletBase through CBullet left untouched
    //  (0x6F2BB580): if the +0xA0 ref resolves, calls a deep,
    //  SEH-guarded impact-reconfirmation closure (0x6F2BE8B0, out of this
    //  session's depth budget) with it and the same 1.0f constant
    //  (`g_slopeThreshold1`) OnArrival's own BuildImpactContext call uses,
    //  then tail-calls this object's own Method_0xA8 (slot 42) *virtually*
    //  (a genuine vtable redispatch on `this`, not the direct call slots
    //  13/42 above use).  Best-effort - see missile_core.cpp.
    virtual void Method_0xBC();
    //  slot 49 / +0xC4 (0x6F2BBAA0) - overrides CBullet's own
    //  GetImpactApplyFlag (0x6F051710, a bare `xor eax,eax`/`retn`
    //  constant, zero stack args) with a genuine forward: if the +0xA0 ref
    //  resolves, tail-jumps into *that resolved object's own* vtable slot
    //  0xB8 - a different, unrelated class's vtable (whatever the +0xA0
    //  ref actually points at), reached only by byte offset, not this
    //  hierarchy's own OnTargetRefNotify, which merely happens to share
    //  the same 0xB8 offset in a completely different vtable; otherwise
    //  returns 0.  Both paths agree on zero stack args, matching
    //  GetImpactApplyFlag's own fixed contract everywhere else in this
    //  hierarchy.  See missile_core.cpp.
    virtual int GetImpactApplyFlag() const;

    //  No standalone constructor/destructor address survives in this
    //  dump's own closure to call *into* - 0x6F0A4210 (below) is itself
    //  the fully-inlined chain (CBullet's own base-construction inlined,
    //  then this class's own vtable stamp and m_floatD/m_floatE/
    //  m_fieldA0Handle/m_fieldA4TypeTag/m_reservedA8 field stores).  An
    //  ordinary compiler-generated default constructor reproduces it
    //  exactly given the in-class initialisers below, the same reasoning
    //  bullet.h's own empty `CBullet() {}` already documents.
    CMissile()
        : m_fieldA0Handle((unsigned int)-1)
        , m_fieldA4TypeTag(-1)
        , m_reservedA8(0)
    {}

    FloatMini    m_floatD;            // +0x90
    FloatMini    m_floatE;            // +0x98
    unsigned int m_fieldA0Handle;     // +0xA0 - "+0xA0" ref, sentinel -1
    int          m_fieldA4TypeTag;    // +0xA4 - "+0xA0" ref, sentinel -1
    unsigned int m_reservedA8;        // +0xA8 - zeroed, write-only elsewhere
};

#endif
