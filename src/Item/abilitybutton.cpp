//============================================================================
//  CAbilityButton's own trivial constant slots (GetAgileTypeId, GetClassName).
//  See abilitybutton.h.  There is no dedicated constructor address - it is
//  fully inlined into CAbilitySpell::CAbilitySpell (abilitybutton.h's own
//  header comment) - so nothing to define here for it.
//============================================================================
#include "abilitybutton.h"

//----------------------------------------------------------------------------
//  0x6F0241F0 - slot 7.  `mov eax, 41416274h; retn` - the multichar literal
//  'AAbt' (A=0x41,A=0x41,b=0x62,t=0x74), a different family tag than
//  CPower's own 'powr'.
//----------------------------------------------------------------------------
unsigned int CAbilityButton::GetAgileTypeId() const
{
    return 'AAbt';
}

//----------------------------------------------------------------------------
//  0x6F0241C0 - slot 22.
//----------------------------------------------------------------------------
const char* CAbilityButton::GetClassName()
{
    return "CAbilityButton";
}
