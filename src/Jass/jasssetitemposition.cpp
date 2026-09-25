//============================================================================
//  0x6F2B6AE0 - JASS_SetItemPosition_callee1: the function JASS's
//  SetItemPosition native (0x6F3C4C80, out of this dump's scope) calls just
//  before returning, per user_knowledge.json's own ground truth for
//  0x6F2B6AE0.  `this` (ecx) is the item; the three stack arguments are the
//  target X and Y (each by address) and a caller flag this function only
//  forwards.
//
//  X and Y are `CFloat` (the software float), not hardware `float`: the
//  shipped code copies each one out of its pointer with a plain integer
//  `mov`, never an `fld`/`fstp` pair, which is the copy MSVC emits for a
//  one-int struct and never for a `float` (docs/msvc-vc8-idioms.md,
//  "CFloat copies move as integers").  They stay encoded all the way into
//  the vtable[0x180] call and are never read back afterwards - the position
//  this function actually publishes comes back out of the item through
//  vtable[0xB8], not out of these two arguments.
//
//  Does this call CWidget::MoveForward (0x6F2ABFB0) or CWidget::MoveTo
//  (0x6F2AC220)?  No - neither address appears anywhere in this 634-function
//  dump (checked directly), and the three CItem-own-vtable calls this root
//  function makes are at +0x180 (index 96), +0xE4 (index 57) and +0x80
//  (index 32), not MoveTo/MoveForward's own slots 0xD8/0xDC (widget.h,
//  user_knowledge.json's CItem vtable dump).  Repositioning here instead
//  goes through a *second*, smaller vtable belonging to whatever object
//  CSelectable::m_reserved28 points at (selectable.h) - reached once through
//  CItem's own vtable[0xB8] accessor (the same "handle-bearing sub-object"
//  widget.cpp's MoveTo/MoveForward already use to read facing, matching
//  layout with SHandleWithType, timesyncbounds.h) to fetch the item's
//  current world position, and once more directly off the field to push the
//  new position and a freshly-built terrain orientation matrix at that
//  sub-object's own vtable slots 0x18/0x1C (itemplacementdispatch.cpp).
//  See docs/targets/JASS_SetItemPosition_callee1.md for the full writeup.
//============================================================================
#include "item.h"
#include "game.h"
#include "fvec3.h"
#include "itemplacementdispatch.h"
#include "timesyncbounds.h"

extern const CFloat g_CFloatDegToRad;   // dword_6FAAE5E8

CFloat __fastcall GetBuildingAngle();
void __fastcall GetItemGroundOffset(FVec3* out, float worldX, float worldY);

double cos(double value);
double sin(double value);
#pragma intrinsic(cos)
#pragma intrinsic(sin)

//============================================================================
//  0x6F2B6270 - BuildItemGroundOrientation: build a 3x3 orientation matrix
//  (row-major: forward, right, ground-normal) for placing an item on the
//  terrain.  Look up the ground offset/normal at the target world (x, y)
//  (0x6F012850, itemgroundlookup.cpp), then combine it with cos/sin of a
//  facing angle into two more, mutually-perpendicular unit rows.  Real x87
//  arithmetic throughout (FVec3, not CFloat) - this matrix feeds a
//  rendering/placement transform, not the deterministic simulation.
//
//  It lives *here*, `static`, in its one caller's translation unit, and not
//  in a file of its own - which is the opposite of this repo's usual "one
//  TU per module" rule, so it needs its reason on the record.
//
//  The shipped function takes its 36-byte output matrix in ESI and cleans
//  its three float arguments itself (`retn 0Ch`).  No declared MSVC calling
//  convention passes a return buffer in esi - but MSVC does not have to use
//  a declared one here.  At /O2 it is free to invent a convention for a
//  function with internal linkage whose every call site it can see, and
//  given exactly that (`static`, one caller, same TU) VC8 picks esi for the
//  buffer on its own.  `__stdcall` on top of that keeps the callee-cleanup
//  of the three floats.  Together those reproduce the shipped ABI exactly -
//  `ret 12`, buffer in esi - where the previous split-TU spelling built a
//  __cdecl `ret 0` with the buffer as a fourth stack argument.
//
//  The price, which is not avoidable: `static` means no external symbol, so
//  this function cannot be JMP-hooked on its own - the injection DLL
//  resolves every replacement by export name (dllmain.cpp's
//  GetProcAddress), and there is nothing to export.  That is the same
//  property that buys the esi convention, so there is no spelling with
//  both.  It is still the right way round: an externally-visible spelling
//  builds `retn 0` against the shipped `retn 0Ch` and corrupts its caller's
//  frame the moment it *is* hooked.  Its code still runs live whenever this
//  function is hooked, because this function calls it.  See
//  docs/notes/hook-abi-crash-classes.md.
//
//  That was not a cosmetic gap.  0x6F2B6270's real caller is the shipped
//  0x6F2B6AE0, which pushes three floats and expects the callee to clean
//  them; a hook that cleans nothing leaves that caller's esp twelve bytes
//  low for the rest of its body, and it dies in its own epilogue.  See
//  tools/abi_audit.py, which is what found it.
//
//  The whole body is now derived from the dump instruction by instruction,
//  including the prologue: both cross products, the order of the two
//  FVec3::Normalize() calls (0x6F011550), the identity the output matrix is
//  pre-filled with, and - the part that was actually wrong in the game - the
//  zeroing of the ground vector before the lookup writes into it.  The row
//  order is read off the nine stores at 6F2B6391..6F2B63C0 rather than
//  assumed: [esi+0x00]=forward, [esi+0x0C]=right, [esi+0x18]=up.
//  See docs/targets/JASS_SetItemPosition_callee1.md.
//============================================================================
static SOrientationMatrix __stdcall
BuildItemGroundOrientation(float worldX, float worldY, float facingAngle)
{
    //  Both of these are initialised, and both initialisations are real
    //  instructions in the shipped prologue (6F2B6273..6F2B62A1) - not a
    //  decompiler's idea of tidiness:
    //
    //    6F2B6273  fld1                      ; 1.0
    //    6F2B6278  fst  dword ptr [esi]      ; m[0] = 1
    //    6F2B627E  fldz                      ; 0.0
    //    6F2B6280  fst  dword ptr [esi+4]    ; m[1..3] and m[5..7] = 0
    //    ...
    //    6F2B6292  fst  [esp+38h+var_18]     ; up.x = 0
    //    6F2B6296  fst  [esp+38h+var_14]     ; up.y = 0
    //    6F2B629A  fstp [esp+38h+var_10]     ; up.z = 0
    //    6F2B629E  fst  dword ptr [esi+10h]  ; m[4] = 1
    //    6F2B62A1  fstp dword ptr [esi+20h]  ; m[8] = 1
    //
    //  var_18/var_14/var_10 are the vector handed to GetItemGroundOffset in
    //  ecx (`lea ecx, [esp+38h+var_18]`, 6F2B627A) and read back as the third
    //  matrix row at the end (6F2B63A1..6F2B63C0), so that really is `up` and
    //  it really is zeroed before the call.
    //
    //  GetItemGroundOffset returns without writing anything at all when the
    //  world point falls outside the Storm singleton's table or lands in a
    //  cell that carries no record - its own comment in itemgroundlookup.cpp
    //  already said "its one caller is what zeroes the vector, before the
    //  call", and this was that caller not doing it, so `up` was an
    //  uninitialised read on that path.
    //
    //  It is worth being exact about what this does and does not fix, because
    //  the first version of this comment claimed more.  It is NOT the visible
    //  wrong item angle.  On the lookup's hit path the callee overwrites all
    //  three components, so zeroed and uninitialised are indistinguishable;
    //  only the miss path can differ.  And the miss path cannot be the common
    //  one, because FVec3::Normalize has no zero guard (fvec3normalize.cpp:
    //  1.0f / sqrt(0) is inf, and 0 * inf is NaN) - on a miss the *shipped*
    //  function hands back a NaN basis, which is not what vanilla does to
    //  items.  So this is a matching fix and an uninitialised read closed,
    //  and the angle is somewhere else.
    //
    //  The identity is dead by the last instruction - all nine slots are
    //  overwritten at 6F2B6391..6F2B63C0 - but MSVC cannot prove that: esi is
    //  the caller's buffer and GetItemGroundOffset is an opaque call that
    //  might read it.  The stores survive /O2 in the shipped code, so they
    //  have to be spelled here too.
    SOrientationMatrix matrix;
    matrix.m[0] = 1.0f;  matrix.m[1] = 0.0f;  matrix.m[2] = 0.0f;
    matrix.m[3] = 0.0f;  matrix.m[4] = 1.0f;  matrix.m[5] = 0.0f;
    matrix.m[6] = 0.0f;  matrix.m[7] = 0.0f;  matrix.m[8] = 1.0f;

    FVec3 up;
    up.m_x = 0.0f;
    up.m_y = 0.0f;
    up.m_z = 0.0f;
    GetItemGroundOffset(&up, worldX, worldY);

    float cosA = (float)cos(facingAngle);
    float sinA = (float)sin(facingAngle);

    //  `up x (cosA, sinA, 0)` - a vector perpendicular to both the ground
    //  normal and the flat facing direction.  Read off the x87 sequence at
    //  6F2B62E1..6F2B6322, not assumed from the textbook basis construction,
    //  which is what it used to be and which had all three components negated
    //  (it computed exactly -right):
    //
    //    6F2B62E1  fld var_14        ; up.y
    //    6F2B62E7  fldz              ; the facing vector's own zero Z
    //    6F2B62E9  fmul st(1), st    ; up.y * 0
    //    6F2B62F7  fmulp st(2), st   ; sinA * up.z
    //    6F2B62FB  fsubrp st(1), st  ; up.y*0 - sinA*up.z
    //    6F2B62FD  fstp var_24       ; right.x
    //    6F2B6306  fmul st, st(4)    ; cosA * up.z
    //    6F2B630E  fmulp st(5), st   ; up.x * 0
    //    6F2B6312  fsubrp st(4), st  ; cosA*up.z - up.x*0
    //    6F2B6316  fstp var_20       ; right.y
    //    6F2B631C  fmulp st(2), st   ; sinA * up.x
    //    6F2B631E  fmul st, st(4)    ; cosA * up.y
    //    6F2B6320  fsubp st(1), st   ; sinA*up.x - cosA*up.y
    //    6F2B6322  fstp var_1C       ; right.z
    //
    //  The facing vector's zero Z is written out rather than folded away: the
    //  shipped code materialises it with `fldz` and really does multiply by it
    //  twice (6F2B62E9 and 6F2B630E).  Both products are zero, so spelling
    //  them costs nothing numerically - and *not* spelling them, which is what
    //  this file used to do, costs four instructions in the diff.  It is also
    //  what a `0.0f - x` would have hidden: that would have been `fchs`, and
    //  this file is judged on the instruction stream.
    FVec3 right;
    right.m_x = up.m_y * 0.0f - sinA * up.m_z;
    right.m_y = cosA * up.m_z - up.m_x * 0.0f;
    right.m_z = sinA * up.m_x - cosA * up.m_y;

    //  `right x up`, NOT `up x right` - derived from 6F2B6326..6F2B635E, not
    //  assumed. Each component is (right_i * up_j - right_j * up_i), i.e. the
    //  right operand's components are the ones being multiplied by up's:
    //
    //    6F2B632C  fmul st,st(3)     ; var_20 * up.z   = right.y * up.z
    //    6F2B6334  fmul st,st(6)     ; var_1C * up.y   = right.z * up.y
    //    6F2B6336  fsubp st(2),st    ; x = right.y*up.z - right.z*up.y
    //    6F2B6342  fmul st,st(2)     ; right.z * up.x
    //    6F2B634A  fmulp st(5),st    ; right.x * up.z
    //    6F2B634E  fsubrp st(4),st   ; y = right.z*up.x - right.x*up.z
    //    6F2B6358  fmulp st(3),st    ; right.x * up.y
    //    6F2B635A  fmulp st(1),st    ; right.y * up.x
    //    6F2B635C  fsubp st(1),st    ; z = right.x*up.y - right.y*up.x
    //
    //  This was the other half of the sign bug, and fixing only one half
    //  moved the error rather than removing it: with `right` corrected but
    //  this cross product still reversed, one row is negated either way, so
    //  the basis stays mirrored - just about a different axis.
    //
    //  Computed from the *unnormalized* right: both Normalize() calls come
    //  after this cross product, at 6F2B6378 and 6F2B6381, and it is the
    //  second of the two that takes right's own slots (`lea ecx, var_24`).
    //  Normalizing right first - what this file used to do - lands on the
    //  same two directions, since forward just scales by 1/|right| and is
    //  normalized anyway, but it reorders the call stream.  Note which one
    //  goes first: forward.
    FVec3 forward;
    forward.m_x = right.m_y * up.m_z - right.m_z * up.m_y;
    forward.m_y = right.m_z * up.m_x - right.m_x * up.m_z;
    forward.m_z = right.m_x * up.m_y - right.m_y * up.m_x;

    forward.Normalize();
    right.Normalize();

    matrix.m[0] = forward.m_x;
    matrix.m[1] = forward.m_y;
    matrix.m[2] = forward.m_z;
    matrix.m[3] = right.m_x;
    matrix.m[4] = right.m_y;
    matrix.m[5] = right.m_z;
    matrix.m[6] = up.m_x;
    matrix.m[7] = up.m_y;
    matrix.m[8] = up.m_z;
    return matrix;
}


//  CFloat's storage is a plain IEEE-754 bit pattern (CFloat.h) - reading it
//  as a hardware float is a reinterpretation, not a conversion.
static inline float AsHardwareFloat(const CFloat& value)
{
    return *(const float*)&value.m_bits;
}

//  CItem's own vtable[0x180] (index 96, 0x6F266C80 per user_knowledge.json's
//  CItem vtable dump - not in this dump, so its body is unread).  Takes the
//  target X and Y by address (both plainly modifiable in place, matching the
//  clamp-style shape CWidget::MoveTo's own early vtable dispatch already
//  established for a sibling slot) plus *eight* small integer flags, of
//  which this call tree only ever passes literals and the caller's own flag:
//  (1, 1, flag, 0, 0, 0, -1, 0).
//
//  The arity is not a guess and it is not cosmetic.  The shipped call site
//  pushes ten dwords (0x6F2B6AEE..0x6F2B6B20) and the frame is 0x28 bytes
//  shallower on the instruction after `call edx` than on the instruction
//  before it, so the callee - a real, unmodified game function - cleans
//  forty bytes off the stack.  Declaring fewer parameters here makes this
//  function push fewer than the callee pops: when this reconstruction is
//  JMP-hooked into the live game the callee then eats twelve bytes of this
//  frame, `add esp, 40h / retn 0Ch` returns through garbage, and the process
//  dies with a near-null EIP.  That is exactly the `eip=0x2` SIGSEGV the
//  live gate reported for this function, hooked on its own, over several
//  rounds - see docs/notes/jass-setitemposition-live-gate-session.md.
typedef void (__thiscall *Vtbl0x180Fn)(void*, CFloat*, CFloat*, int, int,
                                       int, int, int, int, int, int);

//  CItem's own vtable[0xB8] (index 46) - "hand back the handle-bearing
//  sub-object", the same slot and shape widget.cpp's GetHandleObjectFn
//  already established for CWidget's own MoveTo/MoveForward.
typedef SHandleWithType* (__thiscall *GetHandleObjectFn)(void*);

//  CItem's own vtable[0xE4] (index 57, 0x6F2AB680) - hands back a height as
//  an x87 float in st0 for a world position passed by address, plus three
//  opaque small-integer flags (-2, 0, 0).  The vector handed in is the
//  item's own current X and Y with a *zero* Z, and the float that comes back
//  becomes the Z of the position published at the placement sub-object - the
//  shape of a "drop this to the ground at (x, y)" query, with -2 reading
//  like the same sentinel LookupHandle (sub_6F03FA30) already compares
//  against.  Identified in a later session (CWidget::Method_0xE4, item.cpp,
//  from its own separate dump, CItem__Method_0xE4_0x6F2AB680_calltree_asm.md):
//  a ground-height query forwarded to a deep terrain/collision helper plus
//  this object's own vtable[0xE8] (0x6F266A30, still not identified).
typedef float (__thiscall *Vtbl0xE4Fn)(void*, FVec3*, int, int, int);

//  CItem's own vtable[0x80] (index 32) - no arguments beyond `this`, called
//  last, and its return value is this function's own return value.
//  Identified in a later session (CItem::ClearPendingAndNotify, item.cpp, from its own
//  separate dump, CItem__Method_0x80_0x6F2B6130_calltree_asm.md): clears a
//  pending flag and notifies the placement sub-object and a Storm/Terrain-
//  rendering handle, then forwards to this object's own vtable[0x148]
//  (still not identified) with three literal `1`s.
typedef int (__thiscall *Vtbl0x80Fn)(void*);

int CItem::JASS_SetItemPosition_callee1(const CFloat* pX, const CFloat* pY,
                                         int flag)
{
    CFloat x = *pX;
    CFloat y = *pY;

    ((Vtbl0x180Fn)(*(void***)this)[0x180 / 4])(this, &x, &y, 1, 1, flag,
                                               0, 0, 0, -1, 0);

    //  One expression, not a `handle` local: the shipped code pushes
    //  SmartPtrToWorldVec3's own return buffer *before* the vtable[0xB8]
    //  call that produces its `this`, which MSVC only does when nothing
    //  names the intermediate pointer.
    CWorldVec3 worldPos =
        ((const CPathRef*)((GetHandleObjectFn)(*(void***)this)[0xB8 / 4])(this))
            ->SmartPtrToWorldVec3();

    //  Both components are copied out of the returned vector *as CFloat*
    //  first (a plain integer `mov` each, into a scratch slot of their own)
    //  and only then reinterpreted.  The two extra locals are load-bearing:
    //  reinterpreting straight off `worldPos.m_x` lets MSVC `fld` out of the
    //  return buffer directly and loses the shipped code's own
    //  integer-copy-then-`fld` pair.
    CFloat worldX = worldPos.m_x;
    CFloat worldY = worldPos.m_y;

    //  Ask the item where the ground is under (x, y) before publishing the
    //  position: the probe goes in with a zero Z and the answer replaces it.
    FVec3 groundProbe;
    groundProbe.m_x = AsHardwareFloat(worldX);
    groundProbe.m_y = AsHardwareFloat(worldY);
    groundProbe.m_z = 0.0f;

    float groundZ =
        ((Vtbl0xE4Fn)(*(void***)this)[0xE4 / 4])(this, &groundProbe, -2, 0, 0);

    //  Re-spelled, not read back out of `groundProbe`: the shipped code
    //  reinterprets each of the two scratch slots a second time (a second
    //  `fld` off the very same slot).  Caching the hardware floats in
    //  locals instead keeps them live across the vtable[0xE4] call and costs
    //  the frame slots it does not have.
    FVec3 placementPos;
    placementPos.m_x = AsHardwareFloat(worldX);
    placementPos.m_y = AsHardwareFloat(worldY);
    placementPos.m_z = groundZ;

    //  m_pSprite (+0x28, CWidget's - see selectable.cpp) is re-read at each
    //  of the two dispatch sites, not cached across them: the shipped code
    //  emits `mov ecx, [ebx+28h]` twice, once per call.  Hoisting it into a
    //  local costs a register (and a fourth callee-saved push) that the
    //  shipped frame does not have.
    SetItemPlacementPosition((void*)m_pSprite, placementPos);

    //  One expression: the shipped code hands GetBuildingAngle's own return
    //  buffer (still in eax) straight to operator* as its left operand
    //  (`mov edx, eax`).  Naming the intermediate makes MSVC materialise a
    //  second CFloat and `lea` its address instead.
    CFloat angleRad = GetBuildingAngle() * g_CFloatDegToRad;

    SOrientationMatrix orientation = BuildItemGroundOrientation(
        groundProbe.m_x, groundProbe.m_y, AsHardwareFloat(angleRad));
    SetItemPlacementOrientation((void*)m_pSprite, orientation);

    return ((Vtbl0x80Fn)(*(void***)this)[0x80 / 4])(this);
}
