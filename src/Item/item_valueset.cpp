//============================================================================
//  0x6F2B89E0 / 0x6F2B8A50 - CItem's vtable slots 73 and 74 (+0x124/+0x128),
//  where a plain CWidget leaves both alone.
//
//  The two writers of the FloatMiniB value CItem embeds at +0x54: slot 73
//  sets it outright (clamped at zero from below), slot 74 adds a delta to
//  it and hands back the change that actually took effect.  Both publish
//  through FloatMini's own vtable slot 0 rather than storing the field, and
//  both then ask whether the write has just brought the value down onto its
//  floor - if it has, the whole depletion tail runs
//  (CItem::OnValueFloorReached, item_valuefloor.cpp).
//
//  The floor test is deliberately two-sided: FloatValueIsFarFrom says the
//  value is *not* still above the floor, and the value it had on the way in
//  says it *was*.  Only the transition fires the tail, so a second write
//  once the item is already at zero does nothing.
//
//  `!(old <= floor)` and not `old > floor`: MSVC compiles the two
//  differently (`test ah,1` with `je` against `test ah,5` with `jp` - they
//  differ only for NaN) and it is the negated form that is in the binary.
//  Same story for slot 73's clamp, `!(0 > value)`.  See
//  docs/msvc-vc8-idioms.md.
//
//  Own translation unit: both call FloatValueIsFarFrom
//  (floatmini_compare.cpp) and CItem::OnValueFloorReached
//  (item_valuefloor.cpp) for real, so neither callee can share a file with
//  them.
//============================================================================
#include "item.h"

//  0x6F022270 - floatmini_compare.cpp.
bool __fastcall FloatValueIsFarFrom(const FloatMini* self, const CFloat* target);

//  FloatMini's vtable slot 0 again (floatmini.h), spelled with a *reference*
//  second parameter rather than a pointer: same call, but it lets the
//  caller below hand it a temporary directly.  A local typedef, so nothing
//  else's call sites change - a function-pointer cast has no linkage.
typedef void (__thiscall *NotifyRefFn)(void* self, const CFloat& value, int flag);

//  flt_6FAAE4C4 - the floor both setters compare against, in the same
//  0x6FAAExxx run of simulation constants as g_CFloatZero and
//  g_quadrantEpsilon.  Read only as a hardware float here, never as an
//  encoded CFloat, so it is declared the way the compare uses it.
extern const float g_itemValueFloor;

//----------------------------------------------------------------------------
//  0x6F2B89E0 - slot 73 (+0x124): set the value.
//----------------------------------------------------------------------------
void CItem::SetLife(const CFloat* value)
{
    //  Read before the publish, which is what changes it.
    CFloat oldValue = m_floatB.m_value;

    //  Clamped at zero from below.  A pointer selected between the two
    //  candidates rather than a copied CFloat: the shipped code picks the
    //  *address* it is going to publish (`mov eax, offset dword_6FAAE470`
    //  then conditionally `mov eax, ecx`) and pushes that, where a value
    //  copy would have materialised a stack temporary first.
    const CFloat* published =
        (*(const float*)&g_CFloatZero > *(const float*)value)
            ? &g_CFloatZero : value;

    ((FloatMiniB::NotifyFn)(*(void***)&m_floatB)[0])(
        &m_floatB, (CFloat*)published, 1);

    if (!FloatValueIsFarFrom(&m_floatB, (const CFloat*)&g_itemValueFloor) &&
        !(*(const float*)&oldValue <= g_itemValueFloor))
        OnValueFloorReached();
}

//----------------------------------------------------------------------------
//  0x6F2B8A50 - slot 74 (+0x128): add to the value, return the change.
//
//  The return is `new - old` and not the caller's own `delta`: the publish
//  can clamp, and what comes back is what the value actually moved by.
//----------------------------------------------------------------------------
CFloat CItem::AddLife(const CFloat* delta)
{
    CFloat oldValue = m_floatB.m_value;

    //  The vtable *pointer* read into a local before the addition, not at
    //  the point of the call: the shipped code loads it into a callee-saved
    //  register (`mov ebx,[edi+54h]`) above operator+ and only dereferences
    //  it (`mov edx,[ebx]`) after, which is what a hoisted local gives and
    //  an inline `(*(void***)&m_floatB)[0]` at the call does not.
    //  A named pointer to the member, not `&m_floatB` respelled at each
    //  use: the shipped code takes its address once (`lea esi,[edi+54h]`)
    //  and derives the value's own address off *that* (`lea edx,[esi+4]`),
    //  where reaching through `this` each time gives `lea edi,[ebx+58h]`
    //  and a `mov edx, edi` to move it into place.  Same idiom CItem::Load
    //  uses for the same member.
    FloatMiniB* range = &m_floatB;
    void** floatVtable = *(void***)range;

    //  The sum is an unnamed temporary bound to a const reference, not a
    //  named local whose address is taken: the shipped code pushes the
    //  notify's constant `1` *before* it calls operator+ and hands the
    //  returned buffer straight on as the second argument, which is what
    //  argument evaluation inside the call expression gives.  A named
    //  local computes the sum first, pushes both arguments afterwards, and
    //  needs a stack slot of its own on top of the one the dead `delta`
    //  parameter already provides.
    ((NotifyRefFn)floatVtable[0])(range, range->m_value + *delta, 1);

    if (!FloatValueIsFarFrom(&m_floatB, (const CFloat*)&g_itemValueFloor) &&
        !(*(const float*)&oldValue <= g_itemValueFloor))
        OnValueFloorReached();

    //  Re-read rather than reusing `sum`: OnValueFloorReached publishes a
    //  zero of its own, and the shipped code loads [this+58h] again after
    //  the branch rejoins for exactly that reason.
    //
    //  Both operands copied, for the same reason FloatValueIsFarFrom's own
    //  are (floatmini_compare.cpp): handing operator- the member and the
    //  local directly passes their own addresses and emits no stores,
    //  where the shipped code spills a fresh copy of each into two scratch
    //  slots first.
    return CFloat(m_floatB.m_value) - CFloat(oldValue);
}
