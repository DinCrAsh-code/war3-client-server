//============================================================================
//  0x6F257730 / 0x6F25C7C0 - CUnit's vtable slots 69 (+0x114) and 108
//  (+0x1B0).  See unit.h.
//
//  Two unrelated one-liners that happen to be the only CUnit slots in
//  their own two modules, so neither has anywhere else to live.
//
//  Slot 108 is one of the eleven CUnit adds, and it is the *inverse* of
//  slot 79 (unit_life.cpp): both read bit 8 of the flags word at
//  +0x5C, and this one complements it before masking (`shr` / `not` /
//  `and 1`, not `shr` / `and 1` / `xor 1`) - which is what `~` on the
//  shifted value produces and what negating the masked result does not.
//============================================================================
#include "unit.h"

//----------------------------------------------------------------------------
//  0x6F257730 - slot 69.
//----------------------------------------------------------------------------
int CUnit::Method_0x114()
{
    return (int)m_int24C;
}

//----------------------------------------------------------------------------
//  0x6F25C7C0 - slot 108.
//----------------------------------------------------------------------------
int CUnit::Method_0x1B0()
{
    return ~(m_flags5C >> 8) & 1;
}
