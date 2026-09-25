//============================================================================
//  CAbilitySpellBook: the "Spell Book" ability WC3 uses for any unit or
//  item that holds more than one usable sub-ability at once (a hero's own
//  multi-page spellbook, and the mechanism a "Tome"-style item with
//  several charges/effects would use to grant them) - `??_7CAbilitySpell
//  Book@@6B@` found by this build's own RTTI (live IDA symbol search),
//  the same way CAbilityInventory (abilityinventory.h) was.
//
//  Derives from CAbilitySpell (abilityspell.h; CAbilityButton inlined
//  into that class's own constructor).  Field layout from
//  CAbilitySpellBook::CAbilitySpellBook's own writes (0x6F1F7400):
//    +0xE4  m_slotCount   - 11 (0x0B)
//    +0xE8..+0x110  m_handles[11]  - eleven zeroed dwords, one per
//                    sub-ability slot; not read back by this call tree,
//                    so their real type (a handle? an ability pointer?)
//                    is unconfirmed beyond "a dword"
//    +0x114 m_slotMeta    - CAbilitySpellSlotMeta (abilityspellslotmeta.h),
//                    eleven parallel {-1,-1,0} bookkeeping triples behind
//                    a leading count - ends the class at +0x19C
//    +0x19C m_field0x19C  - 0, the field immediately after m_slotMeta;
//                    not read back by this call tree
//============================================================================
#ifndef ABILITYSPELLBOOK_H
#define ABILITYSPELLBOOK_H

#include "abilityspell.h"
#include "abilityspellslotmeta.h"

class CAbilitySpellBook : public CAbilitySpell
{
public:
    //  0x6F1F7400 - see abilityspellbook.cpp.
    CAbilitySpellBook();

    unsigned int m_slotCount;         // +0xE4
    unsigned int m_handles[11];       // +0xE8
    CAbilitySpellSlotMeta m_slotMeta; // +0x114
    unsigned int m_field0x19C;        // +0x19C
};

#endif
