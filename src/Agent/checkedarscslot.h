//============================================================================
//  0x6F077A00 ctor -> 0x6F075CF0 Assign -> 0x6F1D3160 'Arsc'.  Same
//  throwaway-checked-slot family as Agent/checkedslot_agent7_batch_slot107.h
//  (see that header's own comment for the shape and the unreproducible
//  __except_handler4-shaped frame every ctor in it opens), reached here
//  from CUnit::SetFlagBit8's own sibling (0x6F07B3E0, slot107 closure) via
//  its own factory 0x6F07AA20 rather than from a CUnit::GetAbility_XXX
//  accessor - see Unit/unit_flagbit8_sibling.cpp.
//============================================================================
#ifndef CHECKEDARSCSLOT_H
#define CHECKEDARSCSLOT_H

#include "agent.h"

unsigned int GetFourCC_6F1D3160();

struct SLocalAbilitySlot_Arsc
{
    SLocalAbilitySlot_Arsc(CAgent* candidate);
    ~SLocalAbilitySlot_Arsc() { Release(); }
    SLocalAbilitySlot_Arsc* Assign(CAgent* candidate);
    void Release();
    CAgent* m_value;
};

#endif
