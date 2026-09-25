//============================================================================
//  0x6F26D270 - `retn 0`, CFloat returned by value through the hidden ecx
//  pointer.  CUnit vtable slot 72's own depth-1 closure: negative armor
//  (past a clamp of 20) reads a precomputed table; non-negative armor runs
//  the "Misc"/"DefenseArmor"-configured curve
//  `(armor*factor) / (1.0 + armor*factor)`.
//============================================================================
#include "CFloat.h"
#include "gamecontext.h"

//  0x6F009EE0 - Config/configfloat.cpp.
CFloat __fastcall GetConfigFloat(const char* section, const char* key, int index);

//  dword_6FAB4300 - a 21-entry CFloat table (funcmap.py DATA).
extern const CFloat g_negativeArmorMultiplierTable[21];

//  dword_6FAAE4F4 - CFloat.h's own "encoded 1.0" (widget_vtable_leaves.cpp's
//  own alias).
extern const CFloat g_slopeThreshold1;

CFloat __fastcall ArmorDamageMultiplier(const CFloat& armor)
{
    if (!(*(const float*)&armor.m_bits < *(const float*)&g_CFloatZero.m_bits))
    {
        CFloat factor = GetConfigFloat("Misc", "DefenseArmor", 0);
        CFloat product = armor * factor;
        return product / (g_slopeThreshold1 + product);
    }

    int index = -CFloatToInt(armor);
    if (index > 20)
        index = 20;
    return g_negativeArmorMultiplierTable[index];
}
