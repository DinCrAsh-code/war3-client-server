//============================================================================
//  0x6F2ACF90 - CWidget vtable[0xD4] (index 53): re-place the widget where
//  it already is.
//
//  The immediate neighbour of CWidget::InitSprite (0x6F2ACDE0) in the same
//  module and its counterpart at run time: InitSprite places a widget for
//  the first time from a descriptor, this one re-runs the placement against
//  the position the widget already reports.
//
//  Ask the handle-bearing sub-object for the widget's own world position
//  (CPathRef::SmartPtrToWorldVec3, already reconstructed), forward x to
//  vtable[0xE4] with a "look the terrain type up" -1, three zeroes and a
//  0.0f, then probe the row table at that same cell.
//
//  Own translation unit: ProbeWidgetTerrainCell (0x6F2ABF20) is a real call
//  and lives in the same address neighbourhood.
//============================================================================
#include "widget.h"
#include "game.h"
#include "fvec3.h"

//  By `const&`, not by value: the by-value spelling lets MSVC keep the
//  CFloat in a register and copy it out with an integer `mov`, which loses
//  the shipped code's `fld <slot>` / `fstp <field>` pair entirely.  See
//  docs/msvc-vc8-idioms.md, "An integer `mov` to copy four bytes means it
//  is not a `float`".
static inline float AsHardwareFloat(const CFloat& value)
{
    return *(const float*)&value;
}

typedef CPathRef* (__thiscall *GetHandleObjectFn)(void*);

//  vtable[0xE4] - re-place: the position by address, the terrain type (-1
//  meaning "look it up", the same convention CWidget::MoveForward's own
//  terrain argument uses) and two flags.  *Four* stack arguments, not five:
//  the shipped code's `fldz`/`fstp` writes 0.0f into one of its own frame
//  slots, above the two `push 0`s, not into the argument block - and since
//  the callee cleans its own arguments, declaring a fifth made it clean
//  four bytes this caller never pushed.  That is a live eip=0x0 stack
//  corruption, caught by tools/abi_audit.py as "pushes 24 argument bytes
//  where the target pushes 16" and confirmed by the gate.
//
//  It returns a float, which this caller stores and never reads again.
typedef float (__thiscall *ReplaceFn)(void*, FVec3*, int, int, int);

void __fastcall ProbeWidgetTerrainCell(const CFloat* worldX,
                                       const CFloat* worldY);

//  A member, not the `__fastcall F(CWidget*)` free function a previous
//  session wrote: it is vtable slot 53, and a vtable entry has to name a
//  member or the emitted vtable references a symbol nothing defines.  The
//  two spellings compile to the same code - `this` in ecx either way - so
//  only the mangled name changes.
//
//  Declared `int` in widget.h, not `void`: CUnit's own override of this
//  slot needs a real return value on one of its paths, and C++ requires a
//  covariant return type across an override.  This body never sets one -
//  it falls off the end right after the ProbeWidgetTerrainCell call, the
//  same shape the shipped code has (no `mov eax` between that call and
//  `retn`) - so the wider declared type costs it no instructions; the
//  pragma below is only to keep the warning-clean house rule.  4716 (not
//  4715) is what MSVC actually raises here - "must return a value" is a
//  hard error rather than a warning when *no* path in the function returns
//  one at all, which this one does not (unlike CUnit::RefreshWidgetPlacement's
//  own override, where one path does).
#pragma warning(push)
#pragma warning(disable: 4715 4716)
int CWidget::RefreshWidgetPlacement()
{
    CWidget* self = this;
    void* vself = self;

    //  One expression, with nothing naming the intermediate CPathRef*: the
    //  shipped code pushes SmartPtrToWorldVec3's return buffer *before* the
    //  vtable[0xB8] call that produces its `this`, and it only schedules
    //  that push so early when no local holds the pointer.
    CWorldVec3 pos =
        ((GetHandleObjectFn)(*(void***)vself)[0xB8 / 4])(vself)
            ->SmartPtrToWorldVec3();

    //  A whole-vector copy, not two CFloat locals: the shipped code stores
    //  all three words of the returned position into a second frame slot
    //  triple, including the Z it never reads back - and then drops the
    //  ground height it gets from vtable[0xE4] into that same Z.  Two
    //  independent locals would have let MSVC drop the dead Z store.
    CWorldVec3 probe = pos;

    //  The x87 round trip is the reinterpret: `fld` out of the *copy*'s own
    //  slot (not out of `pos`, which would let MSVC fold the load into the
    //  return buffer) and `fstp` into the hardware-float triple.  MSVC
    //  reuses `pos`'s now-dead slots for `world`, which is why the shipped
    //  frame is only 24 bytes deep.
    //  A local of its own.  The shipped frame is only 24 bytes - it lands
    //  these three `fstp`s on the very slots `pos` occupies, having proved
    //  the return buffer dead - and writing them through an `(FVec3*)&pos`
    //  does reproduce that 24-byte frame here, but it also swaps which of
    //  the two vectors MSVC puts low and costs ten more instructions than
    //  it saves.  This spelling keeps every displacement below `world`
    //  right and pays twelve bytes of frame for it.
    FVec3 world;
    world.m_x = AsHardwareFloat(probe.m_x);
    world.m_y = AsHardwareFloat(probe.m_y);
    world.m_z = 0.0f;

    //  Straight into the CFloat's storage: the answer arrives in st0 and
    //  the shipped code `fstp`s it into the copy's Z word with no
    //  intervening integer move, which is the same "a hardware float and a
    //  CFloat are the same four bytes" identity AsHardwareFloat relies on.
    *(float*)&probe.m_z =
        ((ReplaceFn)(*(void***)vself)[0xE4 / 4])(vself, &world, -1, 0, 0);

    ProbeWidgetTerrainCell(&probe.m_x, &probe.m_y);
}
#pragma warning(pop)
