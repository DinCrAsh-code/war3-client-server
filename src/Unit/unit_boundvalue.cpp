//============================================================================
//  0x6F2783C0 - CUnit's vtable slot 109 (+0x1B4), one of the eleven CUnit
//  adds.  See unit.h.
//
//  Answer the low end of the +0x214 tracked range, but only for a unit
//  that is either flagged (bit 0x800 of CAgentWar3::m_flags) or in state 2
//  at +0x1FC; anything else is a hard zero.
//
//  A `float` in st0, not an int: both exits are an x87 load - `fldz` on
//  the early one, `fld dword ptr [eax]` on the other, where eax is the
//  buffer SUnitTrackedRef::GetLow hands back.  That the getter's own
//  return value is what gets dereferenced here is why unittrackedref.h
//  declares it returning CFloat* rather than void.
//============================================================================
#include "unit.h"
#include "CFloat.h"

float CUnit::Method_0x1B4()
{
    if ((m_flags & 0x800) == 0 && m_int1FC != 2)
        return 0.0f;

    CFloat low(kCFloatNoInit);
    return *(const float*)m_ref214.GetLow(&low);
}
