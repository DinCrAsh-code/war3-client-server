//============================================================================
//  CBullet - `??_7CBullet@@6B@` (0x6F87894C, 50 slots).  RTTI hierarchy
//  (agent_worktrees/classes/0x6F87894C.json, Class Informer import):
//
//      CBullet -> CBulletBase -> CWar3Image -> CAgentWar3 -> CAgent
//              -> CObserver -> TRefCnt
//
//  CBullet extends CBulletBase's 48 slots by two (48-49) and overrides
//  eleven of CBulletBase's own 48 (slots 1, 7, 8, 9, 10, 17, 18, 19, 22,
//  40, 42); the other 37 pass straight through unchanged - CBulletBase's
//  (or an ancestor's) responsibility, not reconstructed again here.
//
//  Layout, from the base-construction chain fully inlined into
//  CMissileDarkSummoning::CMissileDarkSummoning() (0x6F051850 - the only
//  address in this dump's own reachable closure that stamps
//  `??_7CBullet@@6B@`; no address in this closure survives as CBullet's
//  own *standalone* constructor, same "outermost surviving stamp"
//  situation CBulletBase's own header documents one level down):
//
//      +0x78  m_projectilePosition, a real, RTTI-named sub-object -
//             `ProjectilePositionTrack` (Pathfinding/projectilepositiontrack
//             .h, the shipped `ProjectilePosition`, `??_7ProjectilePosition
//             @@6B@` @ 0x6F877F28), not a generic embedded TRefCnt.
//             Confirmed two ways: (1) 0x6F051850 stamps
//             `??_7ProjectilePosition@@6B@` at this offset and then the
//             three-dword {refcount=0, handle=-1, typeTag=-1} pattern
//             `PositionTrack`'s own `__forceinline` constructor already
//             produces (Pathfinding/positiontrack.h); (2) CBullet's own
//             vtable slots 8/9 (below) dispatch through *this* field's own
//             vtable at offsets 0x0C/0x18 - exactly `ProjectilePositionTrack
//             ::AttachFromTable`/`::SubmitAttachRecord`'s own slot indices,
//             not some other type's.  Size 0x10 (vtable + refcount +
//             handle + typeTag), ending exactly where the next field
//             starts.
//      +0x88  m_floatMini, a plain `FloatMini` (Math/floatmini.h) -
//             confirmed by DumpState below calling FloatMini::DumpTo
//             (0x6F4770E0, the same address bulletbase.h's own three
//             FloatMini fields use) on it, and by SaveFlags/LoadFlags
//             below reading/writing its `m_value` through the same
//             WriteCFloat/ReadCFloat + vtable-slot-0 "assign and notify"
//             idiom CBulletBase already uses for its own three.
//
//  Size: 0x90 (0x78 + 0x10 + 0x08).
//============================================================================
#ifndef BULLET_H
#define BULLET_H

#include "bulletbase.h"
#include "../Pathfinding/projectilepositiontrack.h"
#include "floatmini.h"

struct SImpactPoint;

class CBullet : public CBulletBase
{
public:
    //====================================================================
    //  CBullet's vtable contribution.  Declaration order *is* vtable
    //  order, sorted by slot index; see docs/targets/CBullet.md for the
    //  address table.  Slots 48-49 are new past CBulletBase's own 48; the
    //  rest override one of CBulletBase's.
    //====================================================================
    //  slot 1 / +0x04 (0x6F051CD0) - the scalar-deleting destructor.  No
    //  separate non-scalar ~CBullet survives: the real destructor body
    //  (destruct m_floatMini - trivial, elided; restamp m_projectilePosition
    //  down to its own base TRefCnt vtable, the same collapsing-implicit-
    //  destructor shape positiontrack.h's own header documents; tail-call
    //  CBulletBase::~CBulletBase()) is fully inlined here, the outermost
    //  level nothing further derived inlines it into in this dump's own
    //  closure.  See bullet_ctordtor.cpp.
    virtual CBullet* DeleteSelf(int flags);
    //  slot 7 / +0x1C (0x6F2BB400) - a bare constant, same shape as every
    //  other level of this family (agent.h, agentwar3.h, war3image.h,
    //  bulletbase.h all declare one at this slot).
    virtual unsigned int GetAgileTypeId() const;
    //  slot 8 / +0x20 (0x6F051750) - CAgentWar3/CWar3Image never override
    //  this slot themselves (it is CAgent's own "always 0" default,
    //  agent.h); CBullet is the first class in *this* chain to override
    //  it.  Calls the CAgent leaf directly (not virtually), then forwards
    //  {0, context, 0} into m_projectilePosition's own
    //  SubmitAttachRecord(a, b, d) through its vtable slot 6, and always
    //  returns 1.  See bullet_position.cpp.
    virtual int Method_0x20(int context);
    //  slot 9 / +0x24 (0x6F051780) - same shape as slot 8, one level up
    //  the ancestor chain (CAgent::Method_0x24, which itself virtually
    //  redispatches to GetRecordVersion), forwarding {0, context} into
    //  m_projectilePosition's own AttachFromTable(index, table) through
    //  its vtable slot 3 - `context` reinterpreted as a table pointer.
    //  Always returns 1.  See bullet_position.cpp.
    virtual int Method_0x24(int context);
    //  slot 10 / +0x28 (0x6F0517B0) - CAgent::GetRecordVersion(context)
    //  (the shared "always 0" leaf, agent.h) plus eleven, called directly.
    //  Skips CBulletBase's own +10 override entirely - a genuinely
    //  different additive constant reached by a genuinely different call,
    //  not a chained CBulletBase::GetRecordVersion()+1.  See
    //  bullet_position.cpp.
    virtual int GetRecordVersion(int context);
    //  slot 17 / +0x44 (0x6F0517F0) - CBulletBase's own DumpState, then
    //  this class's own m_floatMini through its own FloatMini::DumpTo.
    virtual void DumpState(void* sink);
    //  slot 18 / +0x48 (0x6F0517C0) - CBulletBase's own SaveFlags, then
    //  m_floatMini.m_value through WriteCFloat - same idiom bulletbase.h's
    //  own three FloatMinis already use.
    virtual void SaveFlags(class CDataStoreScratch* store);
    //  slot 19 / +0x4C (0x6F051810) - the LoadFlags-slot counterpart:
    //  CBulletBase's own LoadFlags, then ReadCFloat into a local followed
    //  by m_floatMini's own vtable-slot-0 "assign and notify".
    virtual void LoadFlags(class CDataStore* store);
    //  slot 22 / +0x58 (0x6F2BB3B0) - "CBullet", this level's own
    //  GetClassName answer.
    virtual const char* GetClassName();

    //  slot 40 / +0xA0 (0x6F051720) - overrides CBulletBase's default
    //  "zero the out-buffer" (bulletbase.h): resolves a real 3-float
    //  tracked vector through a direct (non-virtual) call on
    //  m_projectilePosition, `sub_6F47A580` - not itself a vtable slot of
    //  ProjectilePositionTrack's own 7, so it is declared here as a plain
    //  free function rather than added to that class.  Best-effort: the
    //  callee's own deep float-conversion callees (`sub_6F496910`,
    //  `sub_6F6EF000`) are not further reconstructed this session.  See
    //  bullet_position.cpp.
    virtual void GetTrackedVector(float* out3) const;
    //  slot 42 / +0xA8 (0x6F2BCCC0) - a direct (non-virtual) call on
    //  m_projectilePosition (`sub_6F47A860`), then a tail-jump straight
    //  into CBulletBase::Method_0xA8 (0x6F2BCBF0) - this level layers one
    //  extra step on top of the inherited body rather than replacing it.
    //  See bullet_position.cpp.
    virtual void Method_0xA8();

    //  slot 48 / +0xC0 (0x6F2C3CF0) - new past CBulletBase's own 48.
    //  Calls the real (non-virtual) CBulletBase::CreateSpriteFromDescriptor
    //  first, resolves a terrain height under the descriptor's own {x,y}
    //  (the already-known deep terrain query, `sub_6F0126F0`), installs it
    //  on m_projectilePosition (`sub_6F47A560`) and flips a flag there
    //  (`sub_6F47A990`) based on the descriptor's own +0x64, assigns
    //  m_floatMini from the descriptor's own +0x50 field, and forwards to
    //  CMissileThunderBoltRefs::Apply (missile_thunderbolt.h) with a flag
    //  taken from this object's own new slot 49 (below) - a hook a
    //  subclass can override to gate the Apply call, always 0 at this
    //  level.  Best-effort: the deep float-conversion callees inside the
    //  terrain-height path are not further reconstructed this session.
    //  See bullet_impact.cpp.
    virtual void OnImpact(const SWidgetArtDescriptor* desc, SImpactPoint* point);
    //  slot 49 / +0xC4 (0x6F051710) - bare `xor eax,eax`/`retn`, no stack
    //  args: an always-false gate at this level, read by slot 48 above
    //  through this object's own vtable (a genuine virtual redispatch on
    //  `this`, not a direct call - a subclass overriding this slot changes
    //  what flag OnImpact hands to Apply).  Named for slot index (house
    //  style, agentwar3.h) - nothing in this dump's own closure pins a
    //  narrower meaning to it.
    virtual int GetImpactApplyFlag() const;

    //  No standalone constructor/destructor address survives in this
    //  dump's own closure (see the file header) - the base-construction
    //  chain (CBulletBase(), then this class's own vtable stamp,
    //  m_projectilePosition and m_floatMini sub-object constructions) is
    //  fully inlined at every owner constructor that reaches it
    //  (CMissileDarkSummoning::CMissileDarkSummoning, 0x6F051850, is the
    //  one this session's own closure happened to surface).  An ordinary
    //  compiler-generated default constructor reproduces it exactly: both
    //  sub-objects declare their own inline default constructors already
    //  (PositionTrack::PositionTrack, Pathfinding/positiontrack.h;
    //  FloatMini::FloatMini, Math/floatmini.h), so no body is needed here.
    CBullet() {}

    ProjectilePositionTrack m_projectilePosition;   // +0x78
    FloatMini               m_floatMini;             // +0x88
};

#endif
