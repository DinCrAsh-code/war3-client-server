//============================================================================
//  0x6F285DD0 / 0x6F285DF0 - two near-identical CUnit predicates reached
//  from EnsureSelectionCircle's own closure (slot 101, 0x6F28DCF0): visible
//  (QueryVisibleImpl(1), unit_visibilitymask.cpp) *and* one particular bit
//  of the +0x5C flags word set. Nothing else in this tree reads either
//  bit, so what each one actually gates is not established beyond "some
//  per-unit reveal condition EnsureSelectionCircle cares about" - named
//  for the bit they test rather than guessed at.
//============================================================================
#include "unit.h"

//  0x6F285DD0 (`retn 0`).
int CUnit::IsVisibleWithFlagBit25()
{
    return QueryVisibleImpl(1) && (m_flags5C & 0x2000000);
}

//  0x6F285DF0 (`retn 0`).
int CUnit::IsVisibleWithFlagBit26()
{
    return QueryVisibleImpl(1) && (m_flags5C & 0x4000000);
}
