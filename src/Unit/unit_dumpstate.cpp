//============================================================================
//  0x6F26C550 / 0x6F26C440 - CUnit's vtable slots 17 (+0x44, DumpState)
//  and 8 (+0x20).  See unit.h.  Same 0x6F26Cxxx module, and neither calls
//  the other.
//
//  This is the function that gave CUnit its field layout.  It is a flat
//  run of "print this field" calls with no control flow at all, and the
//  *printer* it picks for each one states that field's type: 0x6F472EB0
//  for a plain dword ([IntMini], textsink.h), 0x6F4770E0 for a FloatMini
//  ([FltMini], floatmini_dump.cpp - reconstructed alongside this), 0x6F478550
//  for a handle pair ([RefMini]) and 0x6F4772E0 for a FloatMiniB's
//  {val, min, max} triple.  Forty-five fields between +0x54 and +0x304,
//  every one of them typed by nothing more than which of those four it is
//  handed to.
//
//  The order below is the shipped order and is not tidy - the [RefMini]s
//  come first, then the [FltMini]s with +0xC8 *after* +0xDC, then the
//  dwords with +0x54 fifth rather than first and +0x194 after +0x1D8, then
//  the FloatMiniB, then five more dwords.  It is not a sort of any kind
//  and reordering it would cost every instruction after the first
//  difference, so it is transcribed rather than rationalised.
//
//  The base call is `call sub_6F2AC770` - CWidget's own body, direct and
//  non-virtual.  CSelectable does not override slot 17 (selectable.h), so
//  naming CWidget explicitly is what CUnit's own `CSelectable::DumpState`
//  resolves to and what the shipped code calls.
//============================================================================
#include "unit.h"
#include "floatmini.h"
#include "CFloat.h"
#include "pathmove.h"   // g_slopeThreshold1 (0x6FAAE4F4)

//  0x6FAAE5B4 - funcmap.py's DATA table.  Only ever passed on, never read
//  here, so it stays a plain dword the way floatmini.h's own two do.
extern const unsigned int g_unk6FAAE5B4;

void CUnit::DumpState(void* sink)
{
    CWidget::DumpState(sink);

    m_ref130.DumpTo(sink);
    m_ref13C.DumpTo(sink);
    m_ref174.DumpTo(sink);
    m_ref19C.DumpTo(sink);
    m_ref1A8.DumpTo(sink);
    m_ref1DC.DumpTo(sink);
    m_ref250.DumpTo(sink);
    m_ref304.DumpTo(sink);

    ((FloatMini*)m_fltAC)->DumpTo(sink);
    ((FloatMini*)m_fltD0)->DumpTo(sink);
    ((FloatMini*)m_fltDC)->DumpTo(sink);
    ((FloatMini*)m_fltC8)->DumpTo(sink);

    ((SIntMiniValue*)&m_owningPlayer58)->DumpTo(sink);
    ((SIntMiniValue*)&m_flags5C)->DumpTo(sink);
    ((SIntMiniValue*)&m_int60)->DumpTo(sink);
    ((SIntMiniValue*)&m_int64)->DumpTo(sink);
    ((SIntMiniValue*)&m_int54)->DumpTo(sink);
    ((SIntMiniValue*)&m_intE8)->DumpTo(sink);
    ((SIntMiniValue*)&m_intEC)->DumpTo(sink);
    ((SIntMiniValue*)&m_int114)->DumpTo(sink);
    ((SIntMiniValue*)&m_int150)->DumpTo(sink);
    ((SIntMiniValue*)&m_int154)->DumpTo(sink);
    ((SIntMiniValue*)&m_int158)->DumpTo(sink);
    ((SIntMiniValue*)&m_int15C)->DumpTo(sink);
    ((SIntMiniValue*)&m_int160)->DumpTo(sink);
    ((SIntMiniValue*)&m_int198)->DumpTo(sink);
    ((SIntMiniValue*)&m_int1B8)->DumpTo(sink);
    ((SIntMiniValue*)&m_int1BC)->DumpTo(sink);
    ((SIntMiniValue*)&m_int1C0)->DumpTo(sink);
    ((SIntMiniValue*)&m_int1C4)->DumpTo(sink);
    ((SIntMiniValue*)&m_int1C8)->DumpTo(sink);
    ((SIntMiniValue*)&m_int1CC)->DumpTo(sink);
    ((SIntMiniValue*)&m_int1D0)->DumpTo(sink);
    ((SIntMiniValue*)&m_int1D4)->DumpTo(sink);
    ((SIntMiniValue*)&m_int1D8)->DumpTo(sink);
    ((SIntMiniValue*)&m_int194)->DumpTo(sink);
    ((SIntMiniValue*)&m_int1FC)->DumpTo(sink);
    ((SIntMiniValue*)&m_int200)->DumpTo(sink);

    ((FloatMiniB*)m_fltB204)->DumpTo(sink);

    ((SIntMiniValue*)&m_int248)->DumpTo(sink);
    ((SIntMiniValue*)&m_int24C)->DumpTo(sink);
    ((SIntMiniValue*)&m_intE4)->DumpTo(sink);
    ((SIntMiniValue*)&m_int240)->DumpTo(sink);
    ((SIntMiniValue*)&m_int244)->DumpTo(sink);
}

//============================================================================
//  0x6F26C440 - CUnit's vtable slot 8 (+0x20).
//
//  Slot 9's sibling (unit_fieldgroups.cpp) and the same shape: run the
//  base's own body for a starting index, then hand each embedded
//  sub-object that index and let it describe itself, one higher each time,
//  and return one past the last.
//
//  The difference is which slot it dispatches to and with what.  The
//  movement sub-object at +0x164 gets its own vtable+0x20 and four
//  arguments; the five field groups get vtable+0x18 and seven, and the
//  five differ only in the three constants in the middle - a limit
//  (0 or 0x1160) and three CFloat bounds drawn from g_CFloatZero,
//  g_unk6FAAE628, g_unk6FAAE624 and g_slopeThreshold1.  They are
//  transcribed rather than folded into a helper, because each is a
//  separate call site in the shipped code with its own argument block.
//============================================================================

//  +0x164's own vtable+0x20 (slot 8): (index, context, tag, flag).
typedef void (__thiscall *MovementDescribeFn)(void* self, int index, int context,
                                              const void* tag, int flag);
//  A field group's vtable+0x18 (slot 6): (index, context, limit, lo, mid, hi,
//  flag).  Seven stack arguments - the count is the whole reason this is a
//  typedef and not a guess; see tools/vtable_dispatch_audit.py.
typedef void (__thiscall *GroupDescribeFn)(void* self, int index, int context,
                                           int limit, const void* lo,
                                           const void* mid, const void* hi,
                                           int flag);

int CUnit::Method_0x20(int context)
{
    int index = CWidget::Method_0x20(context);

    ((MovementDescribeFn)(*(void***)&m_movement)[0x20 / 4])(
        &m_movement, index, context, &g_unk6FAAE5B4, 0);
    ++index;
    ((GroupDescribeFn)(*(void***)&m_life)[0x18 / 4])(
        &m_life, index, context, 0,
        &g_CFloatZero, &g_unk6FAAE628, &g_unk6FAAE624, 0);
    ++index;
    ((GroupDescribeFn)(*(void***)&m_mana)[0x18 / 4])(
        &m_mana, index, context, 0x1160,
        &g_CFloatZero, &g_CFloatZero, &g_unk6FAAE624, 0);
    ++index;
    ((GroupDescribeFn)(*(void***)&m_ref214)[0x18 / 4])(
        &m_ref214, index, context, 0,
        &g_CFloatZero, &g_unk6FAAE628, &g_unk6FAAE624, 0);
    ++index;
    ((GroupDescribeFn)(*(void***)&m_ref118)[0x18 / 4])(
        &m_ref118, index, context, 0x1160,
        &g_CFloatZero, &g_CFloatZero, &g_slopeThreshold1, 0);
    ++index;
    ((GroupDescribeFn)(*(void***)&m_refFC)[0x18 / 4])(
        &m_refFC, index, context, 0,
        &g_CFloatZero, &g_unk6FAAE628, &g_unk6FAAE624, 0);
    return index + 1;
}
