//============================================================================
//  The four single-bit m_flags setters CWidget, CSelectable and CItem all
//  reach through the same vtable slots.  See agentwar3.h.
//
//  These four sit in four unrelated address neighbourhoods (0x6F0520B0,
//  0x6F267120, 0x6F267140, 0x6F077E20) even though they belong to one
//  class, and the usual explanation - the linker folded four identical
//  bodies onto whatever twin it found first - is ruled out here: this image
//  was linked without /OPT:ICF (agentdefaults.cpp shows two byte-identical
//  two-instruction functions surviving at two addresses).  What is left is
//  ordinary /Gy COMDAT placement: each of these is its own COMDAT and the
//  linker was free to scatter them, and they differ from each other in the
//  bit constant, so nothing folds regardless.  Grouping them in one
//  translation unit here is safe for the only reason that matters - none of
//  them calls another, so there is no call for inlining to eat.
//============================================================================
#include "widget.h"   // these bodies belong to CWar3Image/CWidget, not CAgentWar3

//----------------------------------------------------------------------------
//  0x6F0520B0 - vtable slot 33 (+0x84).  Set the "pending" bit, the one
//  CWar3Image::ClearPendingAndNotify (selectable.cpp) clears again.  No
//  argument and no clear half: this slot only ever sets.  CItem overrides
//  it (0x6F2B7C20, item_vtable.cpp).
//----------------------------------------------------------------------------
void CWar3Image::SetPending()
{
    m_flags |= 1;
}

//----------------------------------------------------------------------------
//  0x6F267120 / 0x6F267140 / 0x6F077E20 - vtable slots 40/41/42
//  (+0xA0/+0xA4/+0xA8).  Set or clear one bit on a plain int flag.
//
//  The shipped code tests the argument and jumps to a *second* `retn 4`
//  rather than folding both stores into one exit, which is what an
//  if/else over two whole statements gives; a `m_flags = on ? m_flags | b
//  : m_flags & ~b;` ternary produces one store fed by a cmov-free
//  select and one exit instead.
//----------------------------------------------------------------------------
void CWidget::SetFlagBit2(int on)
{
    if (on)
        m_flags |= 2;
    else
        m_flags &= ~2u;
}

void CWidget::SetFlagBit4(int on)
{
    if (on)
        m_flags |= 4;
    else
        m_flags &= ~4u;
}

void CWidget::SetFlagBit8(int on)
{
    if (on)
        m_flags |= 8;
    else
        m_flags &= ~8u;
}
