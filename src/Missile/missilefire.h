//============================================================================
//  CMissileFire - `??_7CMissileFire@@6B@` (0x6F8B5FB4, 51 slots).  RTTI
//  hierarchy (agent_worktrees/classes/0x6F8B5FB4.json, Class Informer
//  import):
//
//      CMissileFire -> CMissile -> CBullet -> CBulletBase -> CWar3Image
//                    -> CAgentWar3 -> CAgent -> CObserver -> TRefCnt
//
//  CMissileFire adds exactly one new vtable slot past CMissile's own 50
//  (confirmed by diffing both classes' agent_worktrees/classes/*.json
//  vtable member lists address-by-address) and overrides nine more of the
//  inherited fifty: slots 1, 7, 10, 17, 18, 19, 22, 47, plus the new slot
//  50.  The other 42 pass through unchanged and are CMissile's (or an
//  ancestor's) responsibility, not reconstructed again here.
//
//  Declared in its own header (unlike CBullet/CMissile, which share
//  bulletbase.h's neighbourhood) because this class adds real fields and a
//  real new virtual, matching CMissile's own precedent of one header per
//  class once a class earns new state.
//
//  Layout, from CMissileFire's own constructor (0x6F0F66A0 - the only
//  address in this dump's own reachable closure that stamps
//  `??_7CMissileFire@@6B@`):
//
//      +0xAC  m_fieldAC - a plain int, zeroed by the constructor.  Written
//             by the new slot 50 override (Method_0xC8, below) from a
//             virtual call's own answer (or -1 if that call's receiver is
//             null); dumped as a bare int (SIntMiniValue, Storm/textsink.h -
//             same dumper unit_dumpstate.cpp's own "[IntMini]" fields use)
//             and saved/loaded as a plain dword.  Read back by this
//             class's own Method_0xBC override.
//      +0xB0  m_fieldB0 - a second plain int, same treatment (zeroed by the
//             constructor, SIntMiniValue-dumped, plain dword save/load,
//             written by Method_0xC8, read by Method_0xBC).
//      +0xB4  m_floatF, a plain FloatMini (Math/floatmini.h) - confirmed by
//             DumpState/SaveFlags/LoadFlags below all treating it through
//             FloatMini::DumpTo/WriteCFloat/ReadCFloat+vtable-slot-0 the
//             identical way CMissile's own two FloatMinis already are.
//             The constructor stamps its vtable and its own m_value field
//             from dword_6FAAE470 - which is CFloat.h's own g_CFloatZero,
//             the same value FloatMini's plain default constructor already
//             uses, so no explicit initialiser is needed for either field.
//      +0xBC  m_floatG, a second plain FloatMini, same treatment.
//
//  Size: 0xC4 (0xAC + 0x04 + 0x04 + 0x08 + 0x08).
//============================================================================
#ifndef MISSILEFIRE_H
#define MISSILEFIRE_H

#include "missile.h"
#include "CFloat.h"

class CMissileFire : public CMissile
{
public:
    //====================================================================
    //  CMissileFire's vtable contribution.  Declaration order *is* vtable
    //  order, sorted by slot index; see docs/targets/CMissileFire.md for
    //  the address table.  Nine of these override one of CMissile's own
    //  50 (only nine of the thirteen CMissile itself changed are touched
    //  again here - see the file header); slot 50 is genuinely new,
    //  extending the table to 51.
    //====================================================================
    //  slot 1 / +0x04 (0x6F0F7210) - the scalar-deleting destructor.  No
    //  separate non-scalar ~CMissileFire survives: the real destructor
    //  body (destruct m_floatF/m_floatG - trivial, elided, same collapsing
    //  shape missile.h's own m_floatD/m_floatE already document; restamp
    //  m_projectilePosition down to TRefCnt's own vtable *directly* -
    //  skipping CBullet's and CMissile's own vtable-slot-1 bodies both,
    //  the same "this level inlines the whole chain" shape CMissile's own
    //  DeleteSelf uses one level down for CBullet/CBulletBase - then
    //  tail-call CBulletBase::~CBulletBase() directly) is fully inlined
    //  here.  See missilefire_ctordtor.cpp.
    virtual CMissileFire* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F0DBB30) - a bare FourCC-shaped constant, same
    //  shape as every other level of this family.
    virtual unsigned int GetAgileTypeId() const;
    //  slot 10 / +0x28 (0x6F0F66F0) - a direct (non-virtual) call to the
    //  shared CAgent::GetRecordVersion leaf (agent.h, "always 0"), plus
    //  eighteen at this level - skips CMissile's own +14 override
    //  entirely, same "genuinely different additive constant reached by a
    //  genuinely different call" shape every level of this family uses.
    virtual int GetRecordVersion(int context);
    //  slot 17 / +0x44 (0x6F0F6760) - CBulletBase::DumpState (the direct,
    //  non-virtual base call - *not* CMissile::DumpState, so this level
    //  re-does CBullet's and CMissile's own field dumps inline rather than
    //  chaining through them, the same "skip straight to CBulletBase"
    //  shape CMissile's own DumpState already uses one level down), then
    //  m_floatMini/m_fieldA0.../m_floatD/m_floatE/m_fieldAC/m_fieldB0/
    //  m_floatF/m_floatG, in that field order.  See missilefire_dump.cpp.
    virtual void DumpState(void* sink);
    //  slot 18 / +0x48 (0x6F0F6700) - same base-call shape as DumpState:
    //  CMissile::SaveFlags directly (this level *does* chain through the
    //  immediate base here, unlike DumpState/LoadFlags - confirmed by the
    //  call target, 0x6F0A42B0, CMissile's own address), then this
    //  class's own m_fieldAC/m_fieldB0 through plain WriteDword, then
    //  m_floatF/m_floatG through WriteCFloat.  See missilefire_dump.cpp.
    virtual void SaveFlags(class CDataStoreScratch* store);
    //  slot 19 / +0x4C (0x6F0F67E0) - the LoadFlags-slot counterpart:
    //  CMissile::LoadFlags directly (0x6F0A4380, chaining through the
    //  immediate base the same way SaveFlags does), then m_fieldAC/
    //  m_fieldB0 read back through plain ReadDword, then m_floatF/
    //  m_floatG each through ReadCFloat + their own vtable-slot-0 "assign
    //  and notify".  See missilefire_dump.cpp.
    virtual void LoadFlags(class CDataStore* store);
    //  slot 22 / +0x58 (0x6F0DBAE0) - "CMissileFire", this level's own
    //  GetClassName answer.
    virtual const char* GetClassName();
    //  slot 47 / +0xBC (0x6F0F2E60) - overrides CMissile's own real
    //  Method_0xBC (missile.h) rather than the empty nullsub every level
    //  below CMissile left untouched: resolves this missile's own tracked
    //  vector and +0x2C target ref, holds a counted reference to the
    //  target for the duration (SCheckedUnitSlot), forwards to a deep,
    //  out-of-depth-budget leaf (Method_0xBC's own "no-hit report/spawn"
    //  step) with this level's two new fields and FloatMinis, then
    //  tail-calls CMissile::Method_0xBC() directly (non-virtually) as its
    //  own last step.  Best-effort - see missilefire_bc.cpp.
    virtual void Method_0xBC();
    //  slot 50 / +0xC8 (0x6F0E4C50) - genuinely new past CMissile's own
    //  50 slots.  Resolves an answer through the first stack argument's
    //  own +0x38 sub-object (a virtual call on *that* object, vtable slot
    //  0x3B - a different, unrelated class's vtable, reached only by byte
    //  offset), or -1 if that sub-object is null, into m_fieldAC; copies
    //  the second stack argument straight into m_fieldB0; assigns the
    //  third/fourth stack arguments (both `const CFloat*`, forwarded
    //  straight through from this function's own caller - not new locals)
    //  into m_floatF/m_floatG through their own vtable-slot-0 "assign and
    //  notify"; then tail-calls a second deep, out-of-depth-budget leaf
    //  with the first argument.  Best-effort - see missilefire_bc.cpp.
    virtual void Method_0xC8(void* arg0, unsigned int value,
                              const CFloat* floatF, const CFloat* floatG);

    //  No standalone constructor/destructor address survives in this
    //  dump's own closure to call *into* past the header-only chain below
    //  - 0x6F0F66A0 calls CMissile::CMissile() (fully inlined at that call
    //  site, so nothing further to invoke), stamps this class's own
    //  vtable, then zeroes m_fieldAC/m_fieldB0 and default-constructs
    //  m_floatF/m_floatG from dword_6FAAE470.  An ordinary
    //  compiler-generated constructor with an in-class initialiser list
    //  reproduces it exactly, the same reasoning missile.h's own `CMissile()`
    //  already documents.  m_floatF/m_floatG both read dword_6FAAE470 for
    //  their own m_value - which is CFloat.h's own g_CFloatZero, so
    //  FloatMini's plain default constructor already reproduces it
    //  exactly with no body needed for those two, the same reasoning
    //  bullet.h's own m_floatMini already documents.
    CMissileFire()
        : m_fieldAC(0)
        , m_fieldB0(0)
    {}

    int       m_fieldAC;   // +0xAC
    int       m_fieldB0;   // +0xB0
    FloatMini m_floatF;    // +0xB4
    FloatMini m_floatG;    // +0xBC
};

#endif
