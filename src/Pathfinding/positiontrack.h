//============================================================================
//  PositionTrack - the shipped `Position` class, RTTI vtable
//  `??_7Position@@6B@` @ 0x6F877DDC (agent_worktrees/classes/0x6F877DDC.json,
//  Position_family_calltree_asm.md).  A small TRefCnt-derived handle to a
//  live CPathTrace: {m_handle, m_typeTag} resolved through the global handle
//  table (LookupHandle, game.h) on every access rather than cached as a raw
//  pointer, so the reference survives the CPathTrace itself being torn down
//  and rebuilt underneath it - the identical {handle,typeTag}-pair shape
//  Unit/floatlistener.h's own FloatListener already uses for the same
//  reason, and the same layout independently confirmed against three small
//  owner constructors that embed nothing else near it and touch no field
//  this class doesn't have: 0x6F046130 (CEffectImagePos, +0x84),
//  0x6F051130 (CBaseEffectDetector, +0x84), 0x6F0A1A20
//  (CEffectRangeListener, +0x84) - vtable, then a 0 (refcount), then two -1
//  sentinels (handle, typeTag), 0x10 bytes exactly and nothing more.
//
//  **Named PositionTrack, not Position - a naming accommodation, not a
//  claim of two different classes.**  `??_7Position@@6B@` was already used
//  in this repo by Widget/position.h's own `class Position`, written by an
//  earlier, unrelated session before this class's own vtable/RTTI chain
//  was known.  A first pass of this fix assumed the two were genuinely
//  different, unrelated classes that coincidentally share the literal
//  source name `Position` (MSVC's own mangling cannot distinguish two
//  same-named classes in different translation units, so that was a
//  plausible reading).  **They are not.**  Decoding the actual immediate
//  bytes `CItem::CItem()` (0x6F2B7520) writes at its own `+0x64` gives the
//  literal address `0x6F877DDC` - this exact class's own vtable, not a
//  different one - and the field pattern right after it (vtable, 0, -1, -1)
//  is this class's own layout exactly.  Confirmed further by
//  `Position::StoreTopByteBits` (Widget/position.h's own class, now merged
//  into this one - see below): its body calls
//  `LookupHandle(m_reserved6C, m_reserved70)`, and those two fields are
//  literally this class's own `m_handle`/`m_typeTag` (CPresenceRef,
//  Agent/presenceref.h) at the identical offset. **`Widget/position.h`'s
//  own `Position` and this class are the same shipped type.**  Kept as
//  `PositionTrack` rather than reclaimed as `Position` for now regardless,
//  since the wider rename (this class, `SmartPositionTrack`,
//  `ProjectilePositionTrack`, and every `.cpp`/`agent_worktrees`/
//  `funcmap.py` entry that names them) is a larger, separate mechanical
//  pass not folded into this fix.
//
//  **What was actually wrong in `Widget/position.h`'s own model**, now
//  fixed there: it derived from an invented placeholder,
//  `PositionVtableBase` (a bare 2-method stub with guessed method names and
//  no refcount field), instead of the real `TRefCnt -> CPresenceRef ->
//  CAgentField -> Position` chain this file establishes.  Its own trailing
//  fields (`m_notifyHandle`, `m_placementRef`, etc., `CItem`'s own +0x78
//  onward) are real and correct as declared - not moved here, since ~25
//  already-reconstructed `Item/*.cpp` call sites already read/write them by
//  name as `m_position.<field>` and moving them would touch every one for
//  no correctness gain.  Only the *first* 0x10 bytes (vtable, refcount,
//  handle, typeTag) were mis-modelled; `Widget/position.h`'s `class
//  Position` now derives from this class instead of `PositionVtableBase`,
//  inheriting the real chain and this class's own four extra (non-virtual)
//  methods (`RecordSlot8`/`StoreTopByteBits`/`SetTopByteFlag80`/
//  `SetTopByteBits3070` below) rather than declaring its own copies.
//  `Position::~Position` (`sub_6F03A9E0`, already `EXACT`) is the
//  compiler-*implicit* destructor of that whole chain (no level declares an
//  explicit one) - MSVC collapses the trivial "reset to my own level, call
//  base" sequence at every level down to one direct store of `TRefCnt`'s
//  own vtable, since nothing between construction and that unwind path ever
//  observes an intermediate level's vtable pointer.  Needed only because
//  `CItem::CItem()`'s own SEH frame must be able to unwind an
//  already-constructed `Position` if a later member (one of the two
//  `CAgentTimer`s) throws during construction - not needed by any of the
//  ~29 other owners that just stamp raw bytes with no real typed C++
//  sub-object and hence no unwind obligation.
//
//  SmartPosition/ProjectilePosition sibling sessions should also note that
//  Unit/unit_ctor.cpp carries a *trivial*, throwaway local `class
//  SmartPosition` (a vtable-symbol-only COMDAT stub, explicitly not a real
//  reconstruction) that will need reconciling once `SmartPositionTrack`'s
//  own connection to `CUnit::m_movement` is acted on - see
//  docs/targets/Position-family.md.
//
//  No standalone constructor/destructor address exists *for the general
//  case* - every owner but `CItem` inlines the trivial base construction
//  directly into its own constructor with no real typed C++ sub-object at
//  all (see the family dump's own User notes) - `CItem` above is the one
//  documented exception, because it is the one owner reconstructed as a
//  real, fully-typed C++ class with a real member of this type.
//
//  **Real base classes, corrected.** This class was originally written
//  `: public TRefCnt` directly, with slots 2-5 as its own. IDA's own RTTI
//  base-class list for `SmartPosition` (confirmed by the operator directly
//  against the live database) gives the true chain:
//
//      TRefCnt -> CPresenceRef -> CAgentField -> Position -> SmartPosition
//
//  `CPresenceRef` and `CAgentField` (Agent/presenceref.h, Agent/agentfield.h)
//  add no vtable slots of their own that this class's own 8-slot shape
//  didn't already account for except two still-unreconstructed ones
//  (CPresenceRef's own slots 1/2, CAgentField's own slot-1 override) -
//  slots 2-5 here were always CAgentField's own virtuals, inherited
//  unmodified; this class keeps only its true own slots (1, 6, 7). Same
//  addresses, same bodies, same verdicts throughout - this is an
//  attribution fix, not new reconstruction work.  `CPresenceRef`'s and
//  `CAgentField`'s own unreconstructed slots are deliberately left thunked
//  to their shipped addresses (agentfield.h/presenceref.h) rather than
//  reconstructed here - out of scope for this pass.
//============================================================================
#ifndef POSITIONTRACK_H
#define POSITIONTRACK_H

#include "agentfield.h"  // CAgentField, CHandleObject, LookupHandle, CPathTrace,
                         // CGameData, WorldToGrid, SAgileAgent, SIndexedHandleTable

//----------------------------------------------------------------------------
//  PositionTrack itself.  Declaration order is vtable order (C++'s own
//  rule - see refcnt.h's own note on this).  Slot 0 (ReleaseSelf, TRefCnt's
//  own) and slots 2-5 (CAgentField's own, see agentfield.h) are inherited
//  unmodified and not declared again here.
//----------------------------------------------------------------------------
class PositionTrack : public CAgentField
{
public:
    //  Sets the same three values every raw owner-constructor stamp in this
    //  family writes (vtable is stamped by the owner separately - see this
    //  file's own header comment, "No standalone constructor..."): refcount
    //  0, handle/typeTag -1/-1. Body assignment rather than a member-
    //  initializer-list, since `m_refcount` (TRefCnt) and `m_handle`/
    //  `m_typeTag` (CPresenceRef) are both declared two levels up this
    //  class's own direct base - only a constructor *body* can reach an
    //  indirect base's public data members, not this class's own
    //  initializer list. Exists only for Widget/position.h's own `Position`
    //  (the one real typed `PositionTrack`-derived member in this repo,
    //  CItem::m_position) to inherit through its own delegated base
    //  construction - `__forceinline` for the same reason `Position`'s own
    //  constructor already needs it (see position.h): /Ob2 does not fold a
    //  multi-field constructor into its single call site on its own.
    __forceinline PositionTrack()
    {
        m_refcount = 0;
        m_handle = (unsigned int)-1;
        m_typeTag = (unsigned int)-1;
    }

    //  slot 1 / +0x04 (0x6F03A9F0) - the scalar deleting destructor.
    //  ~PositionTrack() is trivial (no member of this class has one), so it
    //  inlines to exactly the same "reset the vtable to TRefCnt's own,
    //  conditionally SMemFree" shape TRefCnt::DeleteSelf itself has
    //  (Agent/refcnt_deleteself.cpp) - same source shape, different
    //  address, because every class in a TRefCnt chain needs its own slot 1
    //  to invoke the right destructor even when that destructor does
    //  nothing beyond what the base's already does.
    virtual PositionTrack* DeleteSelf(int flags);

    //  slot 6 / +0x18 (0x6F4747B0) - Position's own (SmartPosition and
    //  ProjectilePosition each override this slot separately): clamp
    //  `speed` to the global cap at CGameData's own +0x80
    //  (g_pGameData->m_globalMaxSpeed), convert the chosen value from world
    //  to grid units (WorldToGrid, game.h) and install it as the resolved
    //  CPathTrace's own max speed (CPathTrace::SetMaxSpeed,
    //  Pathfinding/pathtracesetmaxspeed.cpp).  See positiontrackspeed.cpp.
    virtual void SetMaxSpeed(const CFloat* speed);

    //  slot 7 / +0x1C (0x6F474810) - resolve the handle, then hand back the
    //  resolved CPathTrace's own +0x8C (CPathTrace::m_field8C) through the
    //  out-pointer argument.  Position's own.  See positiontrackfield8c.cpp.
    virtual void GetField8C(void** out);

    //  Four ordinary (non-virtual) members - none of these are vtable
    //  slots, all four resolve {m_handle, m_typeTag} (inherited from
    //  CPresenceRef, Agent/presenceref.h) the same way the vtable slots
    //  above do, just by a direct call rather than through the vtable.
    //  Discovered from Widget/position.h's own, now-merged `Position`
    //  class (see this file's own header comment on the merge) - the
    //  reason they were never found by this family's own BFS from the 8
    //  vtable roots is that nothing in the 8 vtable methods' own bodies
    //  ever calls them; they are reached only from unrelated owner code
    //  (CItem's vtable slot 8, CUnit's SUnitMovement, CCameraWar3) that
    //  calls them directly by name, sometimes on a `this` that is not
    //  really a PositionTrack at all but merely shares the same
    //  {handle, typeTag}-at-+0x08/+0x0C shape (Unit/unit_movementflags.cpp's
    //  own header comment documents this "MSVC never distinguishes them,
    //  it only ever sees ecx" reuse directly).

    //  0x6F473980 - build the 0x2C-byte request block CItem's vtable slot 8
    //  hands on, and forward it to SPositionRecordSink::SubmitRecord
    //  (Widget/positionrecordbuild.cpp). `this` is never touched - it
    //  travels in ecx straight through to the callee - which is why the
    //  b==0 short-circuit aside, this could just as well be attached to
    //  any class; it stays here because every other member in this file
    //  builds/reads the same family of position records. See
    //  positiontrackrecordbuild.cpp.
    int RecordSlot8(int a, int b, int c, int d);

    //  0x6F474980 - resolve {m_handle, m_typeTag}, walk to the resolved
    //  object's own +0x94 sub-record, and rewrite the top byte of its
    //  +0x3C dword: clear whatever `mask` selects and OR `value` in,
    //  leaving the low 24 bits alone (load-bearing elsewhere -
    //  CItem::RefreshPlacementRadius writes them through a different
    //  path). See positiontrackflagbyte.cpp.
    void StoreTopByteBits(unsigned char value, unsigned char mask);

    //  0x6F4749E0 - one-bit front end onto StoreTopByteBits: bit 7 on/off.
    //  See positiontrackflagbyte.cpp.
    void SetTopByteFlag80(int on);

    //  0x6F474A20 - another front end onto StoreTopByteBits: set bits 4-5,
    //  clear bit 6 (0x30 under mask 0x70). See positiontrackflagbyte.cpp.
    void SetTopByteBits3070();
};

//----------------------------------------------------------------------------
//  SmartPositionTrack - `SmartPosition` (RTTI `??_7SmartPosition@@6B@` @
//  0x6F877EF0, agent_worktrees/classes/0x6F877EF0.json).  9 slots: the same
//  8 PositionTrack has, slots 1/6/7 overridden, plus a new slot 8 with no
//  PositionTrack/ProjectilePosition counterpart.  See
//  docs/targets/Position-family.md's own "SmartPositionTrack" section for
//  the naming-collision note (Unit/unit_ctor.cpp's own trivial local
//  `SmartPosition` vtable-symbol stub for CUnit::m_movement is very likely
//  this exact shipped class, left untouched per that section).
//
//  No fields of its own: every override below only ever touches
//  {m_handle, m_typeTag} (inherited from CPresenceRef, Agent/presenceref.h)
//  through LookupHandle, or nothing on `this` at all (slot 8).
//----------------------------------------------------------------------------
class SmartPositionTrack : public PositionTrack
{
public:
    //  slot 1 / +0x04 (0x6F03B950) - the scalar deleting destructor.  Same
    //  shape as PositionTrack::DeleteSelf (positiontrackdeleteself.cpp):
    //  ~SmartPositionTrack() is trivial too, so it collapses into the
    //  identical "reset the vtable to TRefCnt's own, conditionally
    //  SMemFree" 13-instruction body at its own address.
    virtual SmartPositionTrack* DeleteSelf(int flags);

    //  slot 6 / +0x18 (0x6F476470) - PositionTrack::SetMaxSpeed's own
    //  clamp-and-convert (positiontrackspeed.cpp), plus one extra call:
    //  the chosen grid-unit speed is also handed to
    //  CPathTrace::SetFieldB4 (Pathfinding/pathtracesetfieldb4.cpp), a new
    //  +0xB4 field with no reader in this call tree.
    virtual void SetMaxSpeed(const CFloat* speed);

    //  slot 7 / +0x1C (0x6F476370) - NOT PositionTrack::GetField8C's own
    //  +0x8C: the raw disassembly reads offset +0xC8 instead
    //  (CPathTrace::m_fieldC8, game.h) - a different, new field this
    //  override alone reaches, otherwise the identical three-instruction
    //  "resolve, hand back through the out-pointer" shape.  See
    //  smartpositiontrackfieldc8.cpp.
    virtual void GetFieldC8(void** out);

    //  slot 8 / +0x20 (0x6F473A00) - SmartPosition's own addition, no
    //  PositionTrack/ProjectilePosition counterpart.  `this` is never
    //  touched (travels in ecx straight through to the callee, the same
    //  "this is a member only because ecx carries it" shape
    //  Widget/position.h's own Position::RecordSlot8 has).  Reached
    //  indirectly through CUnit::m_movement's own vtable+0x20
    //  (Unit/unit_dumpstate.cpp's `MovementDescribeFn`), which is the
    //  concrete evidence m_movement really is a SmartPositionTrack.
    //  See Pathfinding/smartpositiontrackrecordslot8.cpp.
    virtual int RecordSlot8(int index, int context, const void* tag,
                             int flag);
};

#endif
