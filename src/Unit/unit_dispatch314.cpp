//============================================================================
//  0x6F277430 - one of CUnit::Method_0x0C's (slot 3, unit_dispatch.cpp)
//  message-case handlers (jump-table case 852314): drop every ability the
//  unit is currently carrying (SUnitAbilityHost::RemoveAllAbilities,
//  0x6F079F40, unit_abilityhostdrain.cpp), then tail-call slot 0x5C.
//============================================================================
#include "unit.h"

struct SUnitAbilityHost
{
    void RemoveAllAbilities();   // 0x6F079F40, unit_abilityhostdrain.cpp
};

void CUnit::Method_0x0C_DropAllAbilities()
{
    ((SUnitAbilityHost*)this)->RemoveAllAbilities();
    Method_0x5C();
}
