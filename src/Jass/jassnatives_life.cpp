//============================================================================
//  The JASS natives that read and write a widget's *life*.
//
//  Five entry points from the registration table at 0x6F3D4020:
//
//      KillUnit        loc_6F3C8040   (Hunit;)V
//      GetWidgetLife   sub_6F3C4660   (Hwidget;)R
//      SetWidgetLife   sub_6F3C4690   (Hwidget;R)V
//      GetUnitState    sub_6F3C5400   (Hunit;Hunitstate;)R
//      SetUnitState    sub_6F3C5EA0   (Hunit;Hunitstate;R)V
//
//  These are one translation unit because they are one idea, and because
//  the whole family is what settles what CWidget's vtable slots 73-77 and
//  80-81 actually are.  Before this target they were `SetTrackedValue`,
//  `Method_0x12C` and friends - structurally right, semantically blank.
//  `SetWidgetLife` calls slot 73 with the caller's `real`; `GetWidgetLife`
//  calls slot 75 and reads a CFloat back.  Those two natives are named by
//  the shipped registration table itself, so the slots are named by ground
//  truth rather than by inference.  See docs/targets/JASS_KillUnit.md.
//
//  **KillUnit is SetLife(0).**  There is no kill method: the native
//  resolves the unit and hands slot 73 the address of g_CFloatZero.  Every
//  consequence of dying - the death animation, the decay timer, the
//  experience award - hangs off the life pool noticing it has reached its
//  low bound, not off anything this native does.
//
//  `real` in and out.  The table's `R` is an *encoded* CFloat: an argument
//  arrives as a pointer to the 32-bit encoding and a result goes back in
//  eax as the raw bits, never through st(0).  That is why the two getters
//  end in `->m_bits` and the two setters take `const CFloat*`.
//============================================================================
#include "jassnatives.h"
#include "unit.h"
#include "widget.h"
#include "CFloat.h"

//----------------------------------------------------------------------------
//  0x6F3C8040 - KillUnit.
//
//  Hand-dumped: IDA never made a function here, because nothing in the
//  image `call`s a JASS native - the table stores the address.  See the
//  provenance note in asm/JASS_KillUnit_0x6F3C8040_calltree_asm.md.
//
//  `unit->SetLife(...)` and not `((CWidget*)unit)->SetLife(...)`: the call
//  is virtual either way and CUnit overrides the slot, so the emitted
//  `mov edx,[eax]` / `mov eax,[edx+124h]` / `call eax` is the same three
//  instructions the shipped body has.  The null test is on the resolved
//  pointer, not folded into the call.
//----------------------------------------------------------------------------
void __cdecl JASS_KillUnit(int hUnit)
{
    CUnit* unit = ResolveUnitHandle(hUnit);
    if (unit)
        unit->SetLife(&g_CFloatZero);
}

//----------------------------------------------------------------------------
//  0x6F3C4690 - SetWidgetLife.  The general form of the above: any widget
//  with a life pool, and the caller's own value rather than zero.
//----------------------------------------------------------------------------
void __cdecl JASS_SetWidgetLife(int hWidget, const CFloat* value)
{
    CWidget* widget = ResolveWidgetHandle(hWidget);
    if (widget)
        widget->SetLife(value);
}

//----------------------------------------------------------------------------
//  0x6F3C4660 - GetWidgetLife.
//
//  The result is read back through the pointer slot 75 *returns*, not out
//  of the local: the shipped tail is `call edx` / `mov eax,[eax]`, which
//  dereferences the returned buffer.  Writing `life.m_bits` instead would
//  emit a fresh `lea`/load off the frame and lose the instruction.
//
//  kCFloatNoInit for the same reason unit_life.cpp uses it: GetLife fills
//  the buffer before anything reads it, and the zeroing default
//  constructor is one store the shipped code has not got.
//----------------------------------------------------------------------------
int __cdecl JASS_GetWidgetLife(int hWidget)
{
    CWidget* widget = ResolveWidgetHandle(hWidget);
    if (!widget)
        return g_CFloatZero.m_bits;

    CFloat life(kCFloatNoInit);
    return widget->GetLife(&life)->m_bits;
}

//----------------------------------------------------------------------------
//  0x6F3C5EA0 - SetUnitState.
//
//  The range check on `state` is split around the resolve, and that is the
//  shipped order, not an accident: `cmp esi,4` / `jge` happens *before*
//  ResolveUnitHandle is called, and `test esi,esi` / `jl` after it.  So the
//  upper bound is a guard around the whole body and the lower bound is one
//  more condition on the call - which is what the nesting below spells and
//  what a single `if ((unsigned)state < 4)` would not (that is one test).
//----------------------------------------------------------------------------
void __cdecl JASS_SetUnitState(int hUnit, int state, const CFloat* value)
{
    if (state < 4)
    {
        CUnit* unit = ResolveUnitHandle(hUnit);
        if (unit && state >= 0)
            unit->SetState(state, value);
    }
}

//----------------------------------------------------------------------------
//  0x6F3C5400 - GetUnitState.  The same split guard, and the same
//  read-through-the-returned-pointer tail as GetWidgetLife.
//
//  Three separate `return g_CFloatZero.m_bits;` statements, not one guard
//  with `||`.  All three refusals share a single `mov eax, dword_6FAAE470`
//  / `pop esi` / `retn` tail, and the shipped code puts that tail at
//  0x6F3C540A - *between* the first test and the body - reaching it by
//  jumping backwards from the two later tests.  Writing the refusals out
//  separately is what hoists it there; a nested guard sinks one shared tail
//  to the very end instead, which is the same split
//  jassunititeminslot_native.cpp records from the other side (it wants the
//  sunk form and spells the guard nested for it).
//----------------------------------------------------------------------------
int __cdecl JASS_GetUnitState(int hUnit, int state)
{
    if (state >= 4)
        return g_CFloatZero.m_bits;

    CUnit* unit = ResolveUnitHandle(hUnit);
    if (!unit)
        return g_CFloatZero.m_bits;
    if (state < 0)
        return g_CFloatZero.m_bits;

    CFloat value(kCFloatNoInit);
    return unit->GetState(&value, state)->m_bits;
}
