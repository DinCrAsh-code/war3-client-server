//============================================================================
//  0x6F2769B0 - CUnit's vtable slot 95 (+0x17C).  See unit.h.
//
//  Two instructions: hand the unit's FourCC to a type-table probe that
//  looks up the "armorType" column (the callee's own literal) and returns
//  the entry's +0xA8, or zero if the unit type is unknown.  Same shape as
//  slots 88 and 89 (unit_placement.cpp) and, like slot 88's, the callee
//  has no dump heading of its own - IDA has it attached here as a tail
//  chunk - so the score is against the two instructions plus the callee's
//  seven.
//============================================================================
#include "unit.h"

//  0x6F32D370 - the FourCC in ecx and nothing else.
unsigned int __fastcall UnitTypeArmorType_6F32D370(unsigned int fourCC);

unsigned int CUnit::Method_0x17C()
{
    return UnitTypeArmorType_6F32D370(m_footprintType);
}
