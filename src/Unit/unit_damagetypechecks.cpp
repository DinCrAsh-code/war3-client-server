//============================================================================
//  Three small "Misc"-config-gated damage-type predicates, all in the same
//  0x25 5xxx neighbourhood as each other - CUnit vtable slot 72's own
//  depth-1 closure (Method_0x24, the 494-instruction damage-type-modifier
//  root at 0x6F2A40D0).  None calls another.
//============================================================================
#include "gamecontext.h"

//  0x6F009F90 - Config/configint.cpp.
int __fastcall GetConfigInt(const char* section, const char* key, int index);

//----------------------------------------------------------------------------
//  0x6F255630 - `retn 0`.  If "MagicImmunesResistDamage" is off, always
//  false; otherwise, true only for damage type 4.
//----------------------------------------------------------------------------
int __fastcall IsMagicImmuneResistDamageType(int damageType)
{
    if (!GetConfigInt("Misc", "MagicImmunesResistDamage", 0))
        return 0;

    return damageType == 4;
}

//----------------------------------------------------------------------------
//  0x6F255660 - `retn 0`.  Same config gate as above; when it is on, true
//  for every damage type in 1..6 except 4.  The shipped body reaches this
//  through a byte-compressed jump table (jpt_6F255687/byte_6F2556A4) rather
//  than five separate compares, which is exactly what MSVC's own switch
//  lowering does once several case bodies collapse to the same two answers.
//----------------------------------------------------------------------------
int __fastcall IsSpecialArmorDamageType(int damageType)
{
    if (!GetConfigInt("Misc", "MagicImmunesResistDamage", 0))
        return 0;

    switch (damageType)
    {
    case 1:
    case 2:
    case 3:
    case 5:
    case 6:
        return 1;
    default:
        return 0;
    }
}
