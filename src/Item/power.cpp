//============================================================================
//  CPower's own trivial constant slots (GetAgileTypeId, GetClassName).  See
//  power.h.  The constructor has no dedicated address - see power.h's own
//  header comment - so nothing to define here for it.
//============================================================================
#include "power.h"

//----------------------------------------------------------------------------
//  0x6F0293D0 - slot 7.  `mov eax, 706F7772h; retn` - the multichar literal
//  'powr' (p=0x70,o=0x6F,w=0x77,r=0x72), a different family tag than
//  CAbilityInterfaced's own 'AAin' or CAbility's own 'abil'.
//----------------------------------------------------------------------------
unsigned int CPower::GetAgileTypeId() const
{
    return 'powr';
}

//----------------------------------------------------------------------------
//  0x6F029380 - slot 22.
//----------------------------------------------------------------------------
const char* CPower::GetClassName()
{
    return "CPower";
}
