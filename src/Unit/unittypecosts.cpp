//============================================================================
//  0x6F29B800 / 0x6F29B830 - two more probes of the unit-type registry, in
//  the same shape as unittypequeries.cpp's six and reached from the player
//  score's tally walks (Player/playerroster.cpp).
//
//  Two adjacent int fields at +0x20 and +0x24, and the two walks that read
//  them always add them together and multiply the sum by a count - which is
//  the shape of a two-resource price, but that is what the shape suggests
//  and not something this call tree proves, so the names keep the offsets.
//
//  Own translation unit rather than an addition to unittypequeries.cpp for
//  the reason that file's own header gives in reverse: these two are in the
//  0x6F29B8xx module, its six are 0x6F29Bxxx/0x6F29Cxxx, and separating
//  them costs nothing because none of the eight calls another.
//============================================================================
#include "unittypetable.h"

int __fastcall UnitTypeCost20_6F29B800(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    //  The found arm first, as a conditional expression: the shipped `jz`
    //  jumps forward onto a trailing `xor eax, eax`.
    return type != 0 ? type->m_field20 : 0;
}

int __fastcall UnitTypeCost24_6F29B830(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field24 : 0;
}
