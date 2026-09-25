//============================================================================
//  0x6F277A80 - CUnit::IsOwnedByAPlayer.
//
//  Own translation unit: 0x6F277xxx is its own module, the same one
//  SItemHandleFieldObject::DispatchUnknown (itemhandledispatch.h) lives in
//  by address, and unrelated to either of its two callees' own modules.
//
//  No inventory ability at all, or the ability's own "no ownership check"
//  guard ([ability+0x3C] > 0) set, both refuse outright.  Otherwise it reads
//  CAbilityInventory::GetOwnershipLevelField at the ability's current level
//  and asks whether that field is nonzero - the NaN-safe `!=` compare
//  docs/msvc-vc8-idioms.md's own "A float-vs-zero compare's status-word
//  mask" entry documents, against the CFloat global rather than a literal
//  0.0f (the two `fld`s are `[field]` then `dword_6FAAE470`, not `fldz`).
//============================================================================
#include "unitinventory.h"
#include "CFloat.h"

int CUnit::IsOwnedByAPlayer()
{
    CAbilityInventory* inventory = UnitInventory(this);
    if (!inventory || inventory->m_ownershipCheckDisabled > 0)
        return 0;

    CFloat value(kCFloatNoInit);
    CFloat* field = inventory->GetOwnershipLevelField(&value, inventory->m_level);
    return *(const float*)field != *(const float*)&g_CFloatZero;
}
