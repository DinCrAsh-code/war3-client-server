//============================================================================
//  CAbilityInterfaced's own trivial members: the constructor and the two
//  plain-constant slot overrides (GetAgileTypeId, GetClassName).  See
//  abilityinterfaced.h.
//============================================================================
#include "abilityinterfaced.h"

//----------------------------------------------------------------------------
//  0x6F0539D0.  Calls CAbility::CAbility() (0x6F052190, ability.cpp) then
//  re-stamps this class's own vtable - the same "base constructor, then a
//  second vtable store" shape every derived constructor in this chain uses.
//  This class adds no fields, so nothing else to initialize.
//----------------------------------------------------------------------------
CAbilityInterfaced::CAbilityInterfaced()
{
}

//----------------------------------------------------------------------------
//  0x6F025D10 - slot 7.  A different family tag than CAbility's own 'abil'
//  (ability_slots.cpp): 0x4141696E, read low byte first the same way -
//  `'AAin'` packs to that exact value under this compiler's own multichar
//  literal spelling.
//----------------------------------------------------------------------------
unsigned int CAbilityInterfaced::GetAgileTypeId() const
{
    return 'AAin';
}

//----------------------------------------------------------------------------
//  0x6F025CC0 - slot 22.
//----------------------------------------------------------------------------
const char* CAbilityInterfaced::GetClassName()
{
    return "CAbilityInterfaced";
}
