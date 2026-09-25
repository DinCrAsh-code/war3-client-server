//============================================================================
//  0x6F28AC90 - CUnit::CUnit().  See unit.h for the base/member layout.
//  asm/sub_6F28AC90_0x6F28AC90_calltree_asm.md is the dump.
//
//  Same unreproducible __except_handler4-shaped SEH frame as every other
//  multi-sub-object constructor in this family (item.cpp, widget.cpp,
//  agent_ctor.cpp, cameraconstructor.cpp - docs/msvc-vc8-idioms.md, "An
//  __except_handler4-shaped frame this toolchain cannot reproduce") - not
//  reproducible under this build's fixed /GS- /EHs-c-; the body past the
//  frame furniture is the same instructions in the same order: the
//  implicit CSelectable::CSelectable() call, this class's own vtable
//  store, ~150 raw field stores in the shipped shape, the three embedded
//  CAgentTimer::CAgentTimer() calls (automatic - see unit.h), the
//  SCheckedWidgetSlot placement-construction over m_pRef2C4, and the
//  trailing CtorHelper_6F26FAE0() call.
//
//  **CUnit's own vtable store is a manual raw-address write, not left to
//  the compiler's automatic re-stamp** - the same fix item.cpp's own
//  CItem::CItem() applies for the same reason: per
//  tools/ctor_vtable_audit.py's rule, a constructor may stamp a class's
//  own synthesised vtable symbol only when that class's vtable is fully
//  finished, and CUnit's is not (12 of 118 slots are still thunked -
//  docs/targets/CUnit__vtable.md).  The compiler's own automatic base
//  stamp (CSelectable's, transient) and this build's own synthesised
//  ??_7CUnit@@6B@ (never written at all, since this store replaces it)
//  are both dead or absent by the time anything could observe them - the
//  same collapsing logic item.cpp's own header comment documents.
//
//  **The four vtable-bearing sub-object *kinds* stay raw storage, not
//  real typed members.**  unit.h currently declares m_fltAC/m_fltC8/
//  m_fltD0/m_fltDC (`??_7FloatMini@@6B@`), m_fltB204/m_bound210
//  (`??_7FloatMiniB@@6B@`) and m_movement (`??_7SmartPosition@@6B@`) as
//  raw storage / a plain struct, cast to `(FloatMini*)`/`(FloatMiniB*)`
//  at each of their several existing call sites (unit_dumpstate.cpp,
//  unit_fieldgroups.cpp, unit_load.cpp, unit_save.cpp - all already
//  EXACT/IDENTICAL).  Retyping those fields to let the compiler default-
//  construct them for free (the way item.h's real `FloatMiniB m_floatB;`
//  member does for CItem::CItem()) would touch every one of those call
//  sites and risk the "no finished target loses instructions" regression
//  CLAUDE.md's definition of done forbids, with no time in this pass to
//  re-verify each one - see docs/targets/CUnit__vtable.md's own note on
//  this from the session that first laid out this constructor's plan.
//
//  A genuine placement `new` over the raw storage gets the right *field*
//  values but also the generic "was the destination null" branch every
//  placement-new expression carries (docs/msvc-vc8-idioms.md, "Placement
//  new") - which the shipped constructor does not have for these
//  offsets, since they are constant offsets of `this` and never null
//  (measured: adding it back costs this reconstruction several
//  instructions of spurious `cmp`/`je` the dump does not have).  Copying
//  a local instance's own words after ordinary (non-placement)
//  construction reproduces the identical field values with no such
//  branch instead - `StampFloatMini`/`StampFloatMiniB` below - and
//  FloatMiniB's own base-then-derived vtable stamp (FloatMini's
//  constructor sets `??_7FloatMini@@6B@` and one CFloat field;
//  FloatMiniB's own constructor overwrites the same word with
//  `??_7FloatMiniB@@6B@` and adds two more fields) is exactly the double
//  store on m_fltB204 the dump has.  The five `SUnitTrackedRef`
//  sub-objects (+0x98/+0xB8/+0xFC/+0x118/+0x214) and the `SUnitMovement`
//  one (+0x164) *are* already real typed members with named fields, so
//  their own vtable-only stamps (`??_7Float@@6B@`, `??_7SmartPosition@@
//  6B@`) use the same local-class-vtable-copy trick GameUI/
//  cameraconstructor.cpp's own `InitProp<Kind>` already does for
//  `??_7Float@@6B@`/`??_7Angle@@6B@` there - a local class exists purely
//  to reproduce the vtable *symbol*; the real Float/SmartPosition classes
//  are unreconstructed and live entirely in Game.dll.  The genuine
//  out-of-line SCheckedWidgetSlot construction over m_pRef2C4 does keep
//  its placement-`new` null check (there is no non-placement-new way to
//  invoke an out-of-line constructor on existing storage) - see its own
//  comment below and the BEHAVIOUR entry for the one-instruction cost.
//
//  Field values not otherwise commented on read straight off the dump:
//  0/`edi` or -1/`ebx`, the same "zero or all-ones sentinel" shape every
//  other constructor in this family has.  m_reserved290's own +0x294/
//  +0x2A4/+0x2B4 = 1.0f with the other six of the nine = 0.0f reads as a
//  3x3 identity matrix by position (row-major {1,0,0},{0,1,0},{0,0,1}),
//  but nothing in this call tree reads it back to confirm that meaning,
//  so the block stays raw offsets per CLAUDE.md's "do not invent fields
//  the call tree does not read".
//
//  **+0x10C is a 16-bit store** (`mov [esi+10Ch], di`), not the full
//  32-bit `m_int10C` unit.h declares - the shipped constructor only ever
//  clears the field's low word.  **+0x30C is a new field**
//  (`m_int30C`, unit.h) neither DumpState nor ~CUnit's own unwind table
//  reached; this constructor is the first evidence of it (zero).
//
//  **m_bound210 costs one extra instruction.**  unit.h already declares
//  it as a real, separately-named `CFloat` member (read directly by
//  unit_fieldgroups.cpp - see unit.h's own note there), so the compiler
//  default-constructs it automatically (`CFloat() : m_bits(0) {}`,
//  Math/CFloat.h) before this body runs, ahead of the FloatMiniB stamp
//  below that sets its real value - one extra zero-store the shipped
//  constructor does not have, since in the real binary +0x210 is only
//  ever the fourth word of the raw FloatMiniB block, not a separate
//  sub-object of its own.  Not retyped, for the same "don't touch an
//  already-matching call site" reason as the FloatMini fields above.
//============================================================================
#include "unit.h"
#include "agenttypedslots.h"
#include "floatmini.h"
//  Placement new comes from Storm/storm.h (pulled in transitively through
//  unit.h) - <new> is not usable at this build's fixed /Zl.

//----------------------------------------------------------------------------
//  Local stand-ins that exist purely to reproduce the vtable *symbols*
//  ??_7Float@@6B@ / ??_7SmartPosition@@6B@ this constructor stamps into
//  the five SUnitTrackedRef sub-objects and the SUnitMovement one - same
//  technique GameUI/cameraconstructor.cpp's own local Float/Angle already
//  use for CCameraWar3's own SCameraProp members, and Position/
//  PositionVtableBase (Item/position.h) for ??_7Position@@6B@.  The real
//  Float/SmartPosition classes are unreconstructed and live entirely in
//  Game.dll; only the mangled vtable name has to agree.  Deliberately
//  *not* in an anonymous namespace, for the same reason cameraconstructor
//  .cpp gives: that would decorate the mangled symbol, defeating the
//  trick.  Both are trivial/inline (COMDAT), so a second, identical
//  declaration of either elsewhere in this build folds harmlessly at
//  link time rather than colliding.
//----------------------------------------------------------------------------
class Float
{
public:
    virtual void ReleaseSelf() {}
    virtual void OnZeroRefCount() {}
    ~Float() {}
};
class SmartPosition
{
public:
    virtual void ReleaseSelf() {}
    virtual void OnZeroRefCount() {}
    ~SmartPosition() {}
};

//  Stamp one of the five SUnitTrackedRef sub-objects with `??_7Float@@6B@`
//  and the same refcount(0)/handle(-1)/typeTag(-1) triple every one of
//  them gets.
static __forceinline void StampTrackedRef(SUnitTrackedRef& ref)
{
    Float stamp;
    *(void**)&ref = *(void**)&stamp;
    ref.m_refcount = 0;
    ref.m_handle   = (unsigned int)-1;
    ref.m_typeTag  = -1;
}

//  Stamp a raw `FloatMini`-shaped 8-byte block with `??_7FloatMini@@6B@`
//  and its own `m_value` field, by copying a local instance's own words
//  rather than placement-`new`ing `FloatMini` directly onto `dest` (see
//  the file header for why placement `new` is the wrong tool here).
static __forceinline void StampFloatMini(void* dest)
{
    FloatMini stamp;
    *(void**)dest = *(void**)&stamp;
    *(CFloat*)((char*)dest + 4) = stamp.m_value;
}

//  Same idea for the FloatMiniB stamp on m_fltB204/m_bound210 - two
//  separate stamps, base then derived, because the dump shows the field
//  really is overwritten twice (FloatMini's own vtable+value store,
//  immediately followed by FloatMiniB's vtable+min+max one) - the base-
//  then-derived construction order FloatMiniB's own constructor produces
//  for any real instance of it (Math/floatmini.h).
static __forceinline void StampFloatMiniB(void* dest)
{
    StampFloatMini(dest);
    FloatMiniB stamp;
    *(void**)dest = *(void**)&stamp;
    *(CFloat*)((char*)dest + 8)  = stamp.m_min;
    *(CFloat*)((char*)dest + 12) = stamp.m_max;
}

CUnit::CUnit()
{
    //  CUnit's own vtable - see the file header for why this is the
    //  literal, live address rather than the compiler's own automatic
    //  re-stamp to this build's incomplete ??_7CUnit@@6B@.
    *(void**)this = (void*)0x6F931934;

    m_int54          = 0;
    m_owningPlayer58 = 0;
    m_flags5C        = 0;
    m_int60          = 0;
    m_int64          = 0;
    //  +0x68/+0x7C - m_timer68/m_timer7C (CAgentTimer, already real typed
    //  members) are default-constructed automatically ahead of this body,
    //  matching the shipped `call sub_6F0418E0` on each.

    StampTrackedRef(m_life);
    m_pRefA8 = 0;
    StampFloatMini((void*)&m_fltAC);
    m_pRefB4 = 0;
    StampTrackedRef(m_mana);
    StampFloatMini((void*)&m_fltC8);
    StampFloatMini((void*)&m_fltD0);
    m_pRefD8 = 0;
    StampFloatMini((void*)&m_fltDC);
    m_intE4 = 0;
    m_intE8 = 0;
    m_intEC = 0;
    //  +0xF0..+0xFB - m_reservedF0 (unit.h): -1, -1, 0.
    *(unsigned int*)((char*)this + 0xF0) = (unsigned int)-1;
    *(unsigned int*)((char*)this + 0xF4) = (unsigned int)-1;
    *(unsigned int*)((char*)this + 0xF8) = 0;
    StampTrackedRef(m_refFC);
    //  +0x10C - only the low word (see the file header).
    *(unsigned short*)&m_int10C = 0;
    m_pRef110 = 0;
    m_int114  = 0;
    StampTrackedRef(m_ref118);
    m_ref128.m_p = 0;
    m_ref12C.m_p = 0;
    m_ref130.m_t = (unsigned int)-1;
    m_ref130.m_b = (unsigned int)-1;
    m_int138 = 0;
    m_ref13C.m_t = (unsigned int)-1;
    m_ref13C.m_b = (unsigned int)-1;
    //  +0x144/+0x148 - the two dwords of m_reserved144 this call tree
    //  ever writes; +0x14C is untouched.
    *(unsigned int*)((char*)this + 0x144) = 0;
    *(unsigned int*)((char*)this + 0x148) = 0;
    m_int150 = 0;
    m_int154 = 0;
    m_int158 = 0;
    m_int15C = 0;
    m_int160 = 0;

    //  +0x164 - m_movement (SUnitMovement), the `add ecx, 164h` target.
    //  Already a real typed member with named fields; only its vtable-
    //  only stamp needs the local-class trick.
    {
        SmartPosition stamp;
        *(void**)&m_movement = *(void**)&stamp;
        m_movement.m_refcount   = 0;
        m_movement.m_handle     = (unsigned int)-1;
        m_movement.m_handleType = (unsigned int)-1;
    }
    m_ref174.m_t = (unsigned int)-1;
    m_ref174.m_b = (unsigned int)-1;
    m_int17C = 0;
    //  +0x180 - m_timer180 (CAgentTimer) is default-constructed
    //  automatically, matching the shipped `call sub_6F0418E0`.

    m_int194 = 0;
    m_int198 = 0;
    m_ref19C.m_t = (unsigned int)-1;
    m_ref19C.m_b = (unsigned int)-1;
    m_int1A4 = 0;
    m_ref1A8.m_t = (unsigned int)-1;
    m_ref1A8.m_b = (unsigned int)-1;
    m_int1B0 = 0;
    //  m_orderQueueLength (+0x1B4) is not touched by this constructor.
    m_int1B8 = 0;
    m_int1BC = 0;
    m_int1C0 = 0;
    m_int1C4 = 0;
    m_int1C8 = 0;
    m_int1CC = 0;
    m_int1D0 = 0;
    m_int1D4 = 0;
    m_int1D8 = 0;
    m_ref1DC.m_t = (unsigned int)-1;
    m_ref1DC.m_b = (unsigned int)-1;
    //  +0x1E4..+0x1FB - m_reserved1E4, all six dwords zero.
    *(unsigned int*)((char*)this + 0x1E4) = 0;
    *(unsigned int*)((char*)this + 0x1E8) = 0;
    *(unsigned int*)((char*)this + 0x1EC) = 0;
    *(unsigned int*)((char*)this + 0x1F0) = 0;
    *(unsigned int*)((char*)this + 0x1F4) = 0;
    *(unsigned int*)((char*)this + 0x1F8) = 0;
    m_int1FC = 0;
    m_int200 = 0;

    //  +0x204 - m_fltB204/m_bound210 as one 16-byte FloatMiniB: the base-
    //  then-derived double vtable stamp falls out of FloatMiniB's own
    //  constructor for free (see the file header and floatmini.h).
    StampFloatMiniB((void*)&m_fltB204);

    StampTrackedRef(m_ref214);
    m_ref224.m_p = 0;
    //  +0x228..+0x23F - m_reserved228 is not touched by this constructor.
    m_int240 = 0;
    m_int244 = 0;
    m_int248 = 0;
    m_int24C = 0;
    m_ref250.m_t = (unsigned int)-1;
    m_ref250.m_b = (unsigned int)-1;
    m_int258 = 0;

    m_posX284 = 0.0f;
    m_posY288 = 0.0f;
    m_posZ28C = 0.0f;
    //  +0x290 itself is untouched; the eight dwords after it inside
    //  m_reserved290 read as a 3x3 identity matrix by position (see the
    //  file header) - left as raw offsets, per CLAUDE.md.
    *(float*)((char*)this + 0x294) = 1.0f;
    *(float*)((char*)this + 0x298) = 0.0f;
    *(float*)((char*)this + 0x29C) = 0.0f;
    *(float*)((char*)this + 0x2A0) = 0.0f;
    *(float*)((char*)this + 0x2A4) = 1.0f;
    *(float*)((char*)this + 0x2A8) = 0.0f;
    *(float*)((char*)this + 0x2AC) = 0.0f;
    *(float*)((char*)this + 0x2B0) = 0.0f;
    *(float*)((char*)this + 0x2B4) = 1.0f;
    m_motion2B8 = 0.0f;
    m_motion2BC = 0.0f;
    m_motion2C0 = 0.0f;

    //  +0x2C4 - m_pRef2C4 as a SCheckedWidgetSlot(candidate=0), in place.
    //  A genuine out-of-line constructor call (see
    //  agenttypedslots.h/checkedwidgetslot_ctor.cpp), so it has to be a
    //  real placement `new` - and does carry the extra null-check
    //  placement new always adds here (see the file header).
    new ((void*)&m_pRef2C4) SCheckedWidgetSlot(0);

    m_int2C8 = 0;
    //  m_float2CC (+0x2CC) is not touched by this constructor.
    m_int2D4 = 0;
    m_int2DC = 6;
    //  m_animName2E0 (+0x2E0) is not touched by this constructor.
    m_ref304.m_t = (unsigned int)-1;
    m_ref304.m_b = (unsigned int)-1;
    //  +0x30C - see the file header; a new field, not touched anywhere
    //  else in this call tree.
    m_int30C = 0;

    //  The tail-end field-init leaf - see unit.h/unit_ctornotifyfields.cpp.
    //  A real out-of-line call in the shipped build, kept that way here.
    CtorHelper_6F26FAE0();
}
