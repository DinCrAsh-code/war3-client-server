//============================================================================
//  0x6F28D490 / 0x6F28D4B0 - CUnit batch-10, slot 107 closure leaves.  Each
//  tests one bit of +0x5C directly (`this` never needs reloading into ecx
//  for the call - the `test` leaves it untouched, the same idiom
//  unit_visibleflagchecks.cpp's own bit-25/bit-26 predicates document) and
//  only calls through to IsVisibleWithFlagBit25() (0x6F285DD0) when its own
//  bit is set - but note the branch sense on the *call's* result: `jnz`
//  jumps to the "return 0" arm on a *true* result, so this is `bit && !
//  IsVisibleWithFlagBit25()`, not `bit && IsVisibleWithFlagBit25()` (the
//  latter was tried first and scored 0.400, both sides' `jz`/`jnz` on the
//  call's own result inverted against the dump).  The bit being clear
//  short-circuits straight to 0 without ever probing visibility.  Nothing
//  else in this tree reads either bit, so - as with bits 25/26 there -
//  these are named for the bit they gate rather than guessed at.
//
//  Kept in their own translation unit, address-neighbours of each other but
//  not of 0x6F285DD0 (CLAUDE.md's "one TU per original module" - a call
//  compiled in the same TU as its callee's definition gets inlined by
//  /Ob2, which is not what the shipped stream shows: a real `call`).
//============================================================================
#include "unit.h"

//  0x6F28D490 (`retn 0`).
int CUnit::IsFlagBit14AndNotVisibleWithFlagBit25()
{
    return (m_flags5C & 0x4000) && !IsVisibleWithFlagBit25();
}

//  0x6F28D4B0 (`retn 0`).
int CUnit::IsFlagBit15AndNotVisibleWithFlagBit25()
{
    return (m_flags5C & 0x8000) && !IsVisibleWithFlagBit25();
}
