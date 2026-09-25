//============================================================================
//  cunit_agent3 batch - two standalone Assign bodies, same shape as
//  cunit_agent3_checkedslots.h's own thirty-eight-member family (and as
//  SCheckedItemSlot::Assign, itemtypedassign.cpp) but reached without an
//  outer "make one" wrapper in this call tree - each is a bare Assign on
//  a candidate handed in from outside (`arg_0`), not a freshly-made agent.
//
//  Both FourCC getters (GetAbilityAgileTypeId_Amov, 0x6F200960;
//  GetAbilityAgileTypeId_AGbu, 0x6F0D9220) are declared, not defined, here:
//  cross-checked against the other seven agents' own worklists in this
//  parallel run, 0x6F200960 is agent2's own target and 0x6F0D9220 is
//  agent1's - see the write-up.
//============================================================================
#ifndef CUNIT_AGENT3_EXTRA_CHECKEDSLOTS_H
#define CUNIT_AGENT3_EXTRA_CHECKEDSLOTS_H

#include "agent.h"

//  0x6F200960 - 'Amov', MSB first.  Declared, not defined: agent2's own
//  worklist target in this parallel run.
unsigned int GetAbilityAgileTypeId_Amov();

//  0x6F0D9220 - 'AGbu', MSB first.  Declared, not defined: agent1's own
//  worklist target in this parallel run.
unsigned int GetAbilityAgileTypeId_AGbu();

struct SCheckedAbilitySlot_Amov
{
    //  0x6F0DF6C0
    SCheckedAbilitySlot_Amov* Assign(CAgent* candidate);
    CAgent* m_value;
};

struct SCheckedAbilitySlot_AGbu
{
    //  0x6F2756B0
    SCheckedAbilitySlot_AGbu* Assign(CAgent* candidate);
    CAgent* m_value;
};

#endif
