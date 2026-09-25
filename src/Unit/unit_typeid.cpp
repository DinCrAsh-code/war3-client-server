//============================================================================
//  0x6F26C1A0 / 0x6F26C1F0 - CUnit's vtable slots 22 and 7.  See unit.h.
//
//  These two are what pin the class down.  Slot 22 hands back the literal
//  "CUnit" - one class name per class, exactly as CWidget/CSelectable/
//  CItem do - and slot 7 hands back '+w3u' = 0x2B773375, the constant
//  misc_rawcode_getters.cpp had already found sitting 0x30 bytes past its
//  own copy without being able to say which class owned it.
//
//  Same 0x6F26Cxxx module as slots 8 and 17 (unit_dumpstate.cpp); they are
//  split out only because they share no callee with these two and nothing
//  can inline across the boundary either way.
//============================================================================
#include "unit.h"

const char* CUnit::GetClassName()
{
    return "CUnit";
}

unsigned int CUnit::GetAgileTypeId() const
{
    return 0x2B773375;
}
