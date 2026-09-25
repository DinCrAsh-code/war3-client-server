//============================================================================
//  0x6F0E4050 - CAbilityInventory::GetSlotCount.
//
//  Own translation unit, and it has to be: its only caller is
//  CAbilityInventory::GetItemInSlot (0x6F0F0F00,
//  abilityinventoryslot.cpp), and the shipped code calls it out of line.
//  Sharing a TU with that one caller is exactly the case /Ob2 inlines -
//  measured: 0.360 with them together (gen 32 against a target of 18),
//  1.000 apart.  CLAUDE.md's own rule, and the reason it exists.
//============================================================================
#include "abilityinventoryslot.h"
#include "CFloat.h"

//----------------------------------------------------------------------------
//  0x6F0E4050.  How many inventory slots this ability currently has: the
//  per-level "slot count" field, truncated to an integer, capped by the
//  hard maximum at +0x6C.
//
//  The cap is an unsigned `jb`, and the comparison is spelled the way the
//  shipped code branches - keep the computed value only while it is below
//  the cap - rather than as a `min`, which MSVC would emit as a cmov here
//  and the shipped build does not.
//
//  `value` is a real named local and not a temporary folded into the call:
//  the shipped code takes its address (`lea eax,[esp+0Ch+var_4]`), pushes
//  that, and then reads it back through eax after the conversion.
//----------------------------------------------------------------------------
unsigned int CAbilityInventory::GetSlotCount()
{
    CFloat value(kCFloatNoInit);
    unsigned int count = (unsigned int)CFloatToInt(*GetLevelField(&value, m_level));

    unsigned int cap = m_maxSlots;
    if (count >= cap)
        count = cap;

    return count;
}
