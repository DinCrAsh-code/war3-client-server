//============================================================================
//  0x6F26FA70 - CUnit's vtable slot 66 (+0x108).  See unit.h.
//
//  Four instructions: a virtual call into this unit's own slot 113
//  (+0x1C4) with -1, returning what it returns in st0.  CSelectable's body
//  at this slot (0x6F2C74C0) reads a config value; CUnit's answer comes
//  from a slot of its own instead.
//
//  It is the call that fixes slot 113's signature - one stack argument,
//  float result - which nothing else in the sweep could have said.
//============================================================================
#include "unit.h"

float CUnit::Method_0x108()
{
    return Method_0x1C4(-1);
}
