//============================================================================
//  0x6F282F40 - CUnit's slot-3 closure leaf: if the unit's own +0x1F8
//  inventory ability (unitinventory.h's CAbilityInventory,
//  UnitInventory()) is set, OR bit 0x100 into that ability's own +0x20
//  flags word.  +0x20 sits inside CAbilityInventory's own reserved gap
//  (`m_reserved00[0x34]`); nothing else in this call tree reads it back,
//  so it stays a raw offset rather than a named field, per CLAUDE.md's
//  "only touched members get names".
//============================================================================
#include "unit.h"
#include "unitinventory.h"

void CUnit::SetInventoryAbilityFlag0x20Bit0x100()
{
    CAbilityInventory* ability = UnitInventory(this);
    if (ability)
        *(unsigned int*)((char*)ability + 0x20) |= 0x100;
}
