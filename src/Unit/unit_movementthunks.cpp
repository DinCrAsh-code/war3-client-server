//============================================================================
//  0x6F28B0F0 / 0x6F28B100 / 0x6F28B110 / 0x6F28B120 / 0x6F28B130 /
//  0x6F28B140 - CUnit's vtable slots 48-51, 86 and 87.  See unit.h.
//
//  Six forwards onto the movement sub-object at +0x164, all the same two
//  instructions: `add ecx, 164h` then a jump.  That is /O2's tail call for
//  `m_movement.Method(args)`, not a multiple-inheritance adjustor thunk -
//  CUnit has exactly one base, and the offset is a member's, not a base
//  sub-object's.
//
//  widget_vtable_leaves.cpp's note on slots 48-51 read this shape off four
//  of these before CUnit itself was modelled and used it to fix the base
//  class's argument counts; this is the same six functions with an owner.
//
//  **These six are the one place in the sweep where the score is capped by
//  the dump rather than by the reconstruction.**  IDA has each callee's
//  body attached to *this* function as a tail chunk instead of defining it
//  as a function of its own, so `calltree_asm` emits both under one
//  `### sub_6F28B0F0 @ ...` heading and verify.py scores our two
//  instructions against their 26.  The two are what the shipped function
//  is; the callees are separate functions this pipeline currently has no
//  way to address.  See docs/targets/CUnit__vtable.md.
//
//  Own translation unit so the callee declarations cannot be inlined into
//  the forwards, which would leave no jump to match.
//============================================================================
#include "unit.h"

void CUnit::Method_0xC0(int a)
{
    m_movement.ApplyClamped(a);
}

void CUnit::Method_0xC4(int a)
{
    m_movement.Apply(a);
}

void CUnit::Method_0xC8(int a)
{
    m_movement.SetLowNibble(a);
}

void CUnit::Method_0xCC(int a)
{
    m_movement.SetHighNibble(a);
}

int CUnit::GetPlacementMask()
{
    return m_movement.GetPlacementMask();
}

void CUnit::Method_0x15C(unsigned int bits, int a2)
{
    m_movement.SetPlacementBits(bits, a2);
}
