//============================================================================
//  0x6F602DE0 - CControl::CControl.
//
//  The class name is shipped fact, not a role name: this constructor stamps
//  `??_7CControl@@6B@` at +0 and its `_0` twin at +0xB4.  An earlier reading
//  of this batch called it CControlFrame and recorded that "the shipped code
//  never spells its own class name" - it does, in the two vtable symbols the
//  body writes, which is the same evidence CButtonFrame's own name rests on.
//
//  A CSpriteFrame (0x6F60F030) with two vtables, eleven zeroed words at
//  +0x1B0..+0x1DC, a +0x1E0 of 1, and three calls: a state-bit set with 1,
//  a pending-state add with 0, and the dirty-flag or with 2.
//
//  The +0x1D4/+0x1D8 word pair 0x6F601DE0 reads is *not* the one
//  AddPendingState (0x6F601D60) writes - the two sit next to each other and
//  each reads its own neighbour as a mask - which is why both calls are here
//  and why neither can be folded into the other.
//
//  Score capped by the same unified EH frame framecore.cpp records: the
//  shipped body carries an __except_handler4-shaped, cookie-XORed frame that
//  unwinds the CSpriteFrame base, and this toolchain emits none of it at its
//  fixed /GS- /EHs-c-.
//============================================================================
#include "frame.h"

//  0x6F606E60 - Misc/sweep_field_setters.cpp, under the name an earlier
//  sweep gave it.  Re-declared with exactly that file's own receiver type
//  and signature: MSVC puts both in the mangled name, so a tidier spelling
//  here would be a second symbol the linker never joins.
struct SDirtyFlagOwner
{
    void __thiscall OrField124WithField128(int bits);
};

CControl::CControl(int a, int b, int c)
    : CSpriteFrame(a, b, c)
{
    *(const void**)this = &g_vftCControl;
    *(const void**)((char*)this + 0xB4) = &g_vftCControlSecondary;

    m_field1B0 = 0;
    m_field1B4 = 0;
    m_field1B8 = 0;
    m_field1BC = 0;
    m_field1C0 = 0;
    m_field1C4 = 0;
    m_field1C8 = 0;
    m_field1CC = 0;
    m_field1D0 = 0;
    m_stateBits = 0;
    m_stateMask = 0;
    m_field1DC = 0;
    m_field1E0 = 1;

    SetStateBits(1);
    AddPendingState(0);
    ((SDirtyFlagOwner*)this)->OrField124WithField128(2);
}
