//============================================================================
//  CAbilityInterfaced's remaining three overrides (slots 41, 56, 57) - all
//  three reach this class's own new slot 189 (Method_0x2F4, ability.h),
//  which is why they are kept together in one file rather than split by
//  shipped module the way CAbility's own slots are.
//============================================================================
#include "abilityinterfaced.h"

//----------------------------------------------------------------------------
//  0x6F060A30 - slot 41.  Overrides CAbility::Method_6F05F550
//  (ability_slots.cpp): calls the base version first, then this class's own
//  Method_0x2F4(0) - the same qualified-call-to-base shape
//  CAgentWar3::Method_0x24 (agentwar3_slots.cpp) already establishes one
//  class down, just followed by a real second call instead of nothing.
//----------------------------------------------------------------------------
void CAbilityInterfaced::Method_6F05F550()
{
    this->CAbility::Method_6F05F550();
    Method_0x2F4(0);
}

//----------------------------------------------------------------------------
//  0x6F025D40 - slot 56.  Overrides CAbility::Method_0xE0 (a nullsub there,
//  ability.h): forwards straight into Method_0x2F4(0).
//----------------------------------------------------------------------------
void CAbilityInterfaced::Method_0xE0()
{
    Method_0x2F4(0);
}

//----------------------------------------------------------------------------
//  0x6F025D30 - slot 57.  Overrides CAbility::Method_0xE4 (a nullsub there,
//  ability.h): forwards straight into Method_0x2F4(1) - the same shape as
//  Method_0xE0 above, one argument value apart.
//----------------------------------------------------------------------------
void CAbilityInterfaced::Method_0xE4()
{
    Method_0x2F4(1);
}
