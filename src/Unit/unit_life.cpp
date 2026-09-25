//============================================================================
//  CUnit's vtable slots 75-81 (+0x12C..+0x144) - the *life* block.
//
//  Named for what they are since the JASS_KillUnit target: slot 75 is
//  GetLife, 76 GetMaxLife, 77 GetMinLife, 80/81 the two setters, and the
//  SUnitTrackedRef they all forward to is CUnit's hit-point pool.  JASS's
//  GetUnitState routes UNIT_STATE_LIFE to slot 75 and UNIT_STATE_MAX_LIFE
//  to slot 76, which is the proof; see docs/targets/JASS_KillUnit.md.
//  See unit.h and unittrackedref.h.
//
//  Six of the seven are one-line forwards onto the SUnitTrackedRef at
//  +0x98, which is what the shipped `add ecx, 98h` is: not a
//  multiple-inheritance adjustor thunk - CUnit has one base - but /O2's
//  tail call for `m_life.Something(args)` on a member sub-object.  The
//  two setters are the pure form, two instructions and a jump; the two
//  getters have to hold the caller's buffer across the call to hand it
//  back, so they keep a frame.
//
//  Slot 79 is the odd one out and touches none of it: bit 8 of CUnit's own
//  flags word at +0x5C, shifted down.
//
//  Own translation unit rather than sharing unit_thunks.cpp's: nothing
//  here calls anything there, and the file boundary is what stops this
//  compiler inlining SUnitTrackedRef's methods into the forwards and
//  leaving no jump to match - which is the same reason
//  agentwar3_thunks.cpp exists.
//============================================================================
#include "unit.h"

//----------------------------------------------------------------------------
//  0x6F28B2B0 - slot 75 (+0x12C).  Stage the value through a local and copy
//  it out; the shipped code does exactly that rather than handing
//  GetValue the caller's buffer directly.
//----------------------------------------------------------------------------
CFloat* CUnit::GetLife(CFloat* out)
{
    CFloat value(kCFloatNoInit);
    m_life.GetValue(&value);
    *out = value;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F28B080 / 0x6F28B0A0 - slots 76 and 77 (+0x130/+0x134): the high and
//  low ends of the tracked range.
//----------------------------------------------------------------------------
CFloat* CUnit::GetMaxLife(CFloat* out)
{
    m_life.GetHigh(out);
    return out;
}

CFloat* CUnit::GetMinLife(CFloat* out)
{
    m_life.GetLow(out);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F28B2F0 - slot 79 (+0x13C).
//----------------------------------------------------------------------------
int CUnit::Method_0x13C()
{
    return (m_flags5C >> 8) & 1;
}

//----------------------------------------------------------------------------
//  0x6F28B0C0 / 0x6F28B0D0 - slots 80 and 81 (+0x140/+0x144), the setters
//  for the same two ends.
//----------------------------------------------------------------------------
void CUnit::SetMinLife(const CFloat* value)
{
    m_life.SetLow(value);
}

void CUnit::SetMaxLife(const CFloat* value)
{
    m_life.SetHigh(value);
}

//----------------------------------------------------------------------------
//  0x6F28B230 - slot 74 (+0x128).  Add `delta` to the current life value
//  and hand back what it actually moved by - not `*delta` itself, since
//  SetValue (through the resolved CPathTrace's ramp) can clamp.  Same
//  shape as CItem::AddLife (item_valueset.cpp) and CWidget::AddLife
//  (widget_vtable_leaves.cpp): read the value before, publish the sum,
//  notify, read the value after, and return post-minus-pre - except this
//  one goes through the tracked-reference GetValue/SetValue pair rather
//  than a FloatMiniB's own vtable, since +0x98 is a SUnitTrackedRef, not
//  a FloatMiniB.
//
//  Both operands of the final subtraction are copied through an explicit
//  CFloat(...) rather than handed to operator- by address: that is what
//  spills each into its own scratch stack slot instead of taking the
//  local's/member's address directly, the same "copied, not reached
//  through" idiom item_valueset.cpp's own note documents for the same
//  reason.
//----------------------------------------------------------------------------
CFloat CUnit::AddLife(const CFloat* delta)
{
    CFloat pre(kCFloatNoInit);
    m_life.GetValue(&pre);

    //  Read fresh, right at the point of use, rather than reusing `pre`:
    //  the shipped code calls GetValue a second time here even though
    //  nothing changes the value in between, and chains the pointer it
    //  hands back straight into operator+'s first operand instead of
    //  re-forming a local's address.  The same scratch slot serves both
    //  this read and the post-SetValue one below - `current` is dead the
    //  moment `sum` is formed, which is what lets the third read reuse it
    //  instead of taking a fourth stack slot.
    CFloat scratch(kCFloatNoInit);
    CFloat sum = *m_life.GetValue(&scratch) + *delta;
    m_life.SetValue(&sum);

    NotifyLifeChanged();

    m_life.GetValue(&scratch);

    return scratch - pre;
}
