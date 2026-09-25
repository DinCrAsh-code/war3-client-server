//============================================================================
//  CBulletBase - the base of the CBullet/CMissile/CMissileThunderBolt
//  family (`??_7CBulletBase@@6B@`, 0x6F878884, 48 slots).  RTTI hierarchy
//  (agent_worktrees/classes/0x6F878884.json, Class Informer import):
//
//      CBulletBase -> CWar3Image -> CAgentWar3 -> CAgent -> CObserver -> TRefCnt
//
//  CBulletBase extends CWar3Image's 39 slots by nine (39-47) and overrides
//  ten of CWar3Image's own 39 (slots 1, 3, 7, 10, 13, 14, 15, 17, 18, 19,
//  22 - one of them, slot 22 GetClassName, purely to answer with its own
//  name).  The other 28 of CWar3Image's 39 slots pass straight through
//  unchanged - see Widget/war3image.h for those.
//
//  Layout, from the standalone constructor at 0x6F051370 (bulletbase_ctor
//  .cpp - the only address in this call tree that stamps
//  `??_7CBulletBase@@6B@` in isolation, not inlined into a subclass'
//  constructor):
//
//      +0x2C/+0x30  {handle, typeTag} - the "target" reference every
//                   CMissileThunderBoltRefs method (Missile/
//                   missile_thunderbolt.h) already operates on by punning
//                   `this` at this same offset - CBulletBase is that
//                   `this`, not a coincidence: CreateSpriteFromDescriptor
//                   below calls straight into SetTargetRef (0x6F2C2AC0)
//                   on itself.  Sentinel {-1,-1} ("unset"), same convention
//                   itemhandleresolve.h documents.
//      +0x34        unaccounted-for dword, zeroed by the constructor and
//                   touched by nothing else in this class's own reachable
//                   closure - left a reserved gap rather than guessed at.
//      +0x38/+0x3C  two plain dwords CreateSpriteFromDescriptor below fills
//                   from the art descriptor's own +0x3C/+0x40, and
//                   DumpState prints as bare ints (SIntMiniValue - Storm/
//                   textsink.h) - nothing else in this dump's own reachable
//                   closure gives either one a narrower type.
//      +0x40/+0x48/+0x50  three embedded FloatMini objects (Math/
//                   floatmini.h - 8 bytes each, vtable + one CFloat),
//                   default-constructed (g_CFloatZero) and filled in three
//                   times over by CreateSpriteFromDescriptor from the
//                   descriptor's own +0x44/+0x48/+0x4C via each FloatMini's
//                   own vtable slot 0 (a two-argument "assign and notify"
//                   dispatch, the same slot FloatMiniB::SetPair calls
//                   through in item.h's own family).  DumpState confirms
//                   the type: it calls FloatMini::DumpTo (0x6F4770E0) on
//                   all three, byte for byte the same address floatmini.h
//                   already names.
//      +0x58/+0x5C/+0x60  three more plain dwords, same SIntMiniValue
//                   dump shape as +0x38/+0x3C; the descriptor's own
//                   +0x54/+0x58/+0x60 fill the last two of the three (the
//                   descriptor itself skips its own +0x5C for the middle
//                   one - see bulletbase_createsprite.cpp).
//      +0x64        an embedded CAgentTimer (Agent/agenttimer.h, 0x14
//                   bytes) - confirmed by the constructor calling straight
//                   into CAgentTimer::CAgentTimer (0x6F0418E0) at this
//                   offset, Deactivate below calling its ::Cancel()
//                   (0x6F477D30), and this class's own real destructor
//                   (below) calling its ::~CAgentTimer() (0x6F477E20)
//                   there too.
//
//  Size: 0x78 (0x64 + 0x14).
//============================================================================
#ifndef BULLETBASE_H
#define BULLETBASE_H

#include "war3image.h"
#include "agenttimer.h"
#include "floatmini.h"

struct SWidgetArtDescriptor;

//----------------------------------------------------------------------------
//  A message this class's own DispatchMessage (slot 3) reads: `[msg+8]` is
//  the message id it switches on.  Only the one field this dump's own
//  switch reads is named; the rest of the struct is not reached from here.
//----------------------------------------------------------------------------
struct SBulletDispatchMsg
{
    unsigned int m_reserved00;
    unsigned int m_reserved04;
    unsigned int m_id;   // +0x08
};

class CBulletBase : public CWar3Image
{
public:
    //====================================================================
    //  CBulletBase's vtable contribution.  Declaration order *is* vtable
    //  order, sorted by slot index; see docs/targets/CBulletBase.md for
    //  the address table.  Slots 39-47 are new past CWar3Image's own 39;
    //  the rest override one of CWar3Image's.
    //====================================================================
    //  slot 1 / +0x04 (0x6F051CA0) - the scalar-deleting destructor: call
    //  the real destructor below, then - if bit 0 of `flags` is set and
    //  `this` is non-null - free the object through Storm's own allocator.
    //  See bulletbase_ctordtor.cpp.
    virtual CBulletBase* DeleteSelf(int flags);
    //  slot 3 / +0x0C (0x6F2BB4F0) - was mis-filed under
    //  "SMissileThunderBolt::DispatchMessage" (a subclass name that had
    //  nothing to do with this address); it is CBulletBase's own slot 3,
    //  renamed here.  See bulletbase_dispatch.cpp for the dispatch shape
    //  and why it stays DIFFERS (a `switch` cannot reproduce the shipped
    //  byte-remap-table-then-jump-table).
    virtual int DispatchMessage(const SBulletDispatchMsg* msg);
    //  slot 7 / +0x1C (0x6F2BB380) - a bare constant, same shape as
    //  CAgent's/CAgentWar3's/CWar3Image's own (agent.h, agentwar3.h): every
    //  level of this family stamps its own FourCC-shaped id here.
    virtual unsigned int GetAgileTypeId() const;
    //  slot 10 / +0x28 (0x6F051460) - CAgent::GetRecordVersion(context)
    //  (the shared "always 0" leaf, agent.h) plus ten, called directly
    //  rather than virtually (a real `call`, not through the vtable) -
    //  the same "explicit qualified base call" shape CAgentWar3's own
    //  override of this slot already uses one level up, just with a
    //  different additive constant (+10 here, +1 there).
    virtual int GetRecordVersion(int context);
    //  slot 13 / +0x34 (0x6F2BE7B0) - base Deactivate(), cancel the
    //  embedded timer, and stop observing whatever the target ref
    //  currently resolves to (without clearing the ref itself).
    virtual void Deactivate();
    //  slot 14 / +0x38 (0x6F051590) - base Save(store), then the embedded
    //  timer's own vtable slot 8 (an unmodelled CAgentTimer/TRefCnt
    //  virtual - dispatched by raw slot, house style).
    virtual void Save(class CDataStoreScratch* store);
    //  slot 15 / +0x3C (0x6F0515B0) - same shape as Save, timer's own
    //  vtable slot 9.
    virtual void Load(class CDataStore* store);
    //  slot 17 / +0x44 (0x6F051520) - CAgentWar3's own DumpState (agentwar3
    //  .h); CBulletBase overrides it directly with no base call (the base
    //  chain up to CAgent's DumpState is a no-op the compiler elides).
    //  Dumps this class's own ten new fields, in offset order.
    virtual void DumpState(void* sink);
    //  slot 18 / +0x48 (0x6F051470) - CAgentWar3's own SaveFlags; despite
    //  the inherited name this override actually serialises every one of
    //  CBulletBase's own new fields to `store`, not just flag bits - same
    //  vtable slot, wider job at this level.  See bulletbase_save.cpp.
    virtual void SaveFlags(class CDataStoreScratch* store);
    //  slot 19 / +0x4C (0x6F0515D0) - the LoadFlags-slot counterpart,
    //  reading the same fields back plus three sub-object loads through
    //  their own vtable slot 0.
    virtual void LoadFlags(class CDataStore* store);
    //  slot 22 / +0x58 (0x6F2BB330) - "CBulletBase", CAgentWar3's own
    //  GetClassName slot answering with this level's own name.
    virtual const char* GetClassName();

    //  slot 39 / +0x9C (0x6F2C3C40) - new past CWar3Image's own 39.  Calls
    //  the real (non-virtual) CWar3Image::CreateSpriteFromDescriptor
    //  first, then copies this class's own descriptor fields and arms the
    //  target ref and a sprite-controller position callback.  See
    //  bulletbase_createsprite.cpp.
    virtual void CreateSpriteFromDescriptor(const SWidgetArtDescriptor* desc);
    //  slot 40 / +0xA0 (0x6F051420) - default "no tracked vector": zero
    //  the caller's 3-float out-buffer.  Reached polymorphically through
    //  SpriteFollowCallback below (bulletbase_createsprite.cpp), which is
    //  how a subclass overriding this slot with a real position/velocity
    //  answer feeds the sprite's own position-follow controller.
    virtual void GetTrackedVector(float* out3) const;
    //  slot 41 / +0xA4 (0x6F051440) - bare `retn`, no stack args: an
    //  always-true predicate at this level.  Named for slot index (house
    //  style, agentwar3.h) - nothing in this dump's own closure pins a
    //  narrower meaning to it.
    virtual int Method_0xA4() const;
    //  slot 42 / +0xA8 (0x6F2BCBF0) - reads the "Misc"/"BulletDeathTime"
    //  config value, plays this object's own animation (vtable slot 35,
    //  war3image.h) with it, and arms a delayed self-dispatch of message
    //  id 0xD019F through the target-ref observer channel.  Best-effort;
    //  see bulletbase_slot42.cpp.
    virtual void Method_0xA8();
    //  slot 43 / +0xAC (0x6F2BB590) - copy two dwords out of two source
    //  structs (`__thiscall`-shaped adjuster-style receiver, not `this`)
    //  through a shared field-copy helper (0x6F6EEE20). Kept as a raw
    //  slot-dispatch-shaped free function, same reasoning as
    //  CBhPoProjectile's own explicit-vtable-slot idiom.
    virtual void Method_0xAC(void* dstA, void* dstB, void* srcA, void* srcB);
    //  slot 44 / +0xB0 (0x6F051450) - same bare `retn`/always-true shape
    //  as slot 41, different address, no evidence either overrides the
    //  other's meaning.
    virtual int Method_0xB0() const;
    //  slot 45 / +0xB4 (0x6F2BB570) - was mis-filed as a lazy placeholder
    //  ("SVTableSlot0xBCOwner::InvokeSlot0xBC" - a name that pre-dated
    //  knowing which class owned it, and named the wrong offset besides,
    //  0xBC rather than this slot's own 0xB4).  It is CBulletBase's own
    //  slot 45: an explicit tail-jump to whatever sits in *this* object's
    //  own vtable slot 0xBC (slot 47) - i.e. an alias, not a distinct
    //  implementation.  Renamed here; see bulletbase_dispatch.cpp.
    virtual void InvokeSlot0xBC();
    //  slot 46 / +0xB8 (0x6F2BE7E0) - the message-3 handler DispatchMessage
    //  routes event 0xD01A4 (852388) to.  Compares the resolved target ref
    //  against the message's own +0x0C reference and, on a match, sets a
    //  "target lost" bit (0x10000000) in m_flags; either way releases both
    //  locally-resolved references.  __except_handler4-shaped SEH frame
    //  this toolchain cannot reproduce (docs/msvc-vc8-idioms.md) - DIFFERS
    //  regardless of source fidelity.  See bulletbase_ondispose.cpp.
    virtual void OnTargetRefNotify(const SBulletDispatchMsg* msg);
    //  slot 47 / +0xBC (0x6F2BB580, `nullsub_1286`) - confirmed empty from
    //  raw_bytes (a bare 0xC3), not just the IDA name.
    virtual void Method_0xBC();

    //  0x6F051370 - see bulletbase_ctordtor.cpp.  The base-construction
    //  chain (CAgent::CAgent, then CAgentWar3's/CWar3Image's own
    //  m_flags/m_reserved24/m_pSprite zero stores) is fully inlined here,
    //  the same collapsing-stamps shape war3image.h's own constructor
    //  documents - this is the outermost level that survives with a real,
    //  standalone address, because it is the last one nothing further
    //  derived inlines it into in this dump's own closure.
    CBulletBase();

    //  0x6F03E800 - the real (non-scalar) destructor: destructs m_timer,
    //  restamps the vtable to CAgentWar3's own, and tail-calls into the
    //  CAgent/CObserver/TRefCnt chain above.  Not itself a vtable slot -
    //  slot 1's scalar-deleting destructor (DeleteSelf above) is the only
    //  thing that calls it, with a plain `call`, not a virtual dispatch.
    ~CBulletBase();

    unsigned int m_targetHandle;   // +0x2C - target ref, sentinel -1
    int          m_targetTypeTag;  // +0x30 - target ref, sentinel -1
    unsigned int m_reserved34;     // +0x34 - zeroed, untouched otherwise
    unsigned int m_fieldA;         // +0x38 - SIntMiniValue in DumpState
    unsigned int m_fieldB;         // +0x3C - SIntMiniValue in DumpState
    FloatMini    m_floatA;         // +0x40
    FloatMini    m_floatB;         // +0x48
    FloatMini    m_floatC;         // +0x50
    unsigned int m_fieldC;         // +0x58 - SIntMiniValue in DumpState
    unsigned int m_fieldD;         // +0x5C - SIntMiniValue in DumpState
    unsigned int m_fieldE;         // +0x60 - SIntMiniValue in DumpState
    CAgentTimer  m_timer;          // +0x64
};

#endif
