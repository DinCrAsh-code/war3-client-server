//============================================================================
//  0x6F266190 - CDestructable::GetAgileTypeId (vtable slot 7,
//  CAgentWar3::GetAgileTypeId - agentwar3.h), the same bare `mov eax,
//  imm32` shape as every other class in this family
//  (CAgent::GetAgileTypeId, agentdefaults.cpp).
//
//  CUnit's own copy of the same slot (0x6F26C1F0) used to live here beside
//  it under an equally minimal `class CUnit : public CAgentWar3`.  It has
//  a real class now - unit.h, deriving from CSelectable, with all 118
//  vtable slots - so the body moved to unit_typeid.cpp; keeping the stub
//  here would have been a second, differently-shaped definition of the
//  same class in the same program.
//
//  CDestructable now has a real class too - destructable.h, with all 109
//  vtable slots - so this file no longer declares its own minimal
//  `class CDestructable : public CAgentWar3`; it just wires this one body
//  in as the real class's slot 7 override, the way unit_typeid.cpp already
//  does for CUnit.  Kept as its own translation unit rather than merged
//  into destructable_vtable_slots.cpp only because nothing about it
//  changed and there was no reason to touch it.
//============================================================================
#include "destructable.h"

unsigned int CDestructable::GetAgileTypeId() const
{
    return 0x2B773364;
}
