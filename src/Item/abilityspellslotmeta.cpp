//============================================================================
//  CAbilitySpellSlotMeta::CAbilitySpellSlotMeta - see abilityspellslotmeta.h.
//============================================================================
#include "abilityspellslotmeta.h"

//----------------------------------------------------------------------------
//  0x6F1E4D80 - a descending-count loop that still walks the array forward
//  (CLAUDE.md's own "descending count" idiom: comparing the counter
//  against zero is cheaper than comparing against a fixed bound, but the
//  element pointer itself still just increments).
//----------------------------------------------------------------------------
CAbilitySpellSlotMeta::CAbilitySpellSlotMeta()
{
    m_count = 11;

    Entry* entry = m_slots;
    for (int i = 10; i >= 0; i--)
    {
        entry->b = -1;
        entry->a = -1;
        entry->c = 0;
        entry++;
    }
}
