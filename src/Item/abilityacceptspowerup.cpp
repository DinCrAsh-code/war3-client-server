//============================================================================
//  0x6F0E3C50 - CAbilityInventory::AcceptsPowerup.
//
//  Own translation unit, next to CAbilityInventory::IsFull (0x6F0F0E80,
//  abilityisfull.cpp) by address but not sharing one with it: neither calls
//  the other.
//
//  No guard at all here (unlike CUnit::IsOwnedByAPlayer's +0x3C check) -
//  straight to the powerup-acceptance level field at the ability's current
//  level, then the same NaN-safe `!=` against g_CFloatZero.  The result is
//  narrowed through `movzx eax, al` where IsOwnedByAPlayer's own `mov
//  eax,1` is not - both are `int` in C++ either way; the narrowing is a
//  register-width artefact of the compiler proving the comparison result
//  already fits a byte, not a source difference.
//============================================================================
#include "unitinventory.h"
#include "CFloat.h"

int CAbilityInventory::AcceptsPowerup()
{
    CFloat value(kCFloatNoInit);
    CFloat* field = GetPowerupLevelField(&value, m_level);
    bool result = *(const float*)field != *(const float*)&g_CFloatZero;
    return result;
}
