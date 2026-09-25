//============================================================================
//  CUnit's small vtable slots out of the 0x6F28Bxxx module - the
//  neighbourhood that holds most of its trivial overrides.
//
//  They are in their own translation unit for the reason
//  agentwar3_thunks.cpp gives for its pair: a forwarder whose callee is
//  visible gets the callee inlined and there is no `jmp` left to match.
//============================================================================
#include "unit.h"

//----------------------------------------------------------------------------
//  0x6F28B070 - slot 23 (+0x5C).  A bare `jmp sub_6F472990`: CUnit reclaims
//  the slot CSelectable overrides (0x6F472990 is CAgent's own body, which
//  CSelectable does *not* inherit at this index - selectable.h overrides it
//  - so re-stating the base call is the only way back to it).
//----------------------------------------------------------------------------
void CUnit::Method_0x5C()
{
    CAgent::Method_0x5C();
}

//----------------------------------------------------------------------------
//  0x6F28B1E0 - slot 25 (+0x64).  Two instructions: hand back the dword at
//  +0x58 raw.  CAgentWar3's own body at this slot (0x6F001A20) computes
//  something; CUnit just has the answer stored.
//----------------------------------------------------------------------------
unsigned int CUnit::Method_0x64()
{
    return m_owningPlayer58;
}

//----------------------------------------------------------------------------
//  0x6F28B300 / 0x6F28B2D0 - slots 43 and 44 (+0xAC/+0xB0).  Both start
//  from the same visibility probe; slot 44 is nothing else.
//
//  Slot 43's second test reuses the probe's own zero result as the
//  comparison operand (`cmp [ecx+200h], eax` with eax already known 0),
//  which is why it is written against a literal 0 here and comes out as
//  the same two instructions.
//----------------------------------------------------------------------------
int CUnit::GetDefaultCellCentres()
{
    //  A *direct* call, not a dispatch through slot 60: CUnit's own slot
    //  60 (unit_spritevisibility.cpp) is one `jmp` into this same address,
    //  and these two call it straight rather than going back out through
    //  the vtable to reach it.
    //
    //  The comparison is signed - `jle`, not `jbe` - and it is against the
    //  probe's own zero result rather than a fresh literal, which is why
    //  it reads as `> 0` here and comes out as the shipped two
    //  instructions.
    return QueryVisibleImpl(1) == 0 &&
           ((m_flags5C & 0x20000000) == 0 || (int)m_int200 > 0);
}

int CUnit::Method_0xB0()
{
    return QueryVisibleImpl(1);
}

//----------------------------------------------------------------------------
//  0x6F28B0E0 - slot 46 (+0xB8), GetHandleObject.  Two instructions: the
//  address of the movement sub-object at +0x164.  Every "reach through the
//  handle-bearing sub-object" call site in CWidget (widget.cpp's
//  GetHandleObjectFn) lands here for a unit.
//----------------------------------------------------------------------------
SHandleWithType* CUnit::GetHandleObject()
{
    return (SHandleWithType*)&m_movement;
}

//----------------------------------------------------------------------------
//  0x6F28B180 - slot 47 (+0xBC).  A hardware float straight out of +0x2CC,
//  returned in st0.  CWidget's own body at this slot reads a global
//  (widget.h); a unit has its own.
//----------------------------------------------------------------------------
float CUnit::Method_0xBC()
{
    return m_float2CC;
}

//----------------------------------------------------------------------------
//  0x6F28B1D0 - slot 59 (+0xEC), GetOwningPlayerIndex.  The field slot 25
//  hands back too, which is what names it.
//----------------------------------------------------------------------------
int CUnit::GetOwningPlayerIndex()
{
    return (int)m_owningPlayer58;
}

//----------------------------------------------------------------------------
//  0x6F28B330 - slot 67 (+0x10C).  Writes one dword through the caller's
//  out pointer.
//----------------------------------------------------------------------------
int* CUnit::Method_0x10C(int* out)
{
    *out = (int)m_int2D4;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F28B150 - slot 71 (+0x11C).  Two instructions: always zero.  Not a
//  stub - CWidget's own body at this slot computes something, and CUnit
//  deliberately answers no.
//----------------------------------------------------------------------------
int CUnit::Method_0x11C()
{
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F28B160 / 0x6F28B170 - slots 92 and 93 (+0x170/+0x174), the
//  position-notify handle's getter and setter.
//----------------------------------------------------------------------------
int CUnit::GetPositionNotifyHandle()
{
    return (int)m_notifyHandle25C;
}

void CUnit::SetPositionNotifyHandle(int value)
{
    m_notifyHandle25C = (unsigned int)value;
}

//----------------------------------------------------------------------------
//  0x6F28B2E0 - slot 111 (+0x1BC), one of the eleven slots CUnit adds.
//  Bit 4 of CAgentWar3's flags word at +0x20, shifted down.  Slot 62
//  (unit_visibility.cpp) is its only caller in this sweep and tests it as
//  a plain flag, which is what fixes its signature.
//----------------------------------------------------------------------------
int CUnit::Method_0x1BC()
{
    return (m_flags >> 4) & 1;
}
