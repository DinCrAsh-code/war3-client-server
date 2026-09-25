//============================================================================
//  CAbilitySpellSlotMeta: the per-sub-ability-slot bookkeeping array
//  CAbilitySpellBook embeds at +0x114 (abilityspellbook.h) - eleven
//  {-1,-1,0} triples behind a leading count, parallel to the eleven raw
//  handle slots at +0xE8.  Its own out-of-line constructor (0x6F1E4D80),
//  kept in its own TU away from CAbilitySpellBook's own constructor for
//  the usual reason (CLAUDE.md, "must not be inlined into each other").
//============================================================================
#ifndef ABILITYSPELLSLOTMETA_H
#define ABILITYSPELLSLOTMETA_H

class CAbilitySpellSlotMeta
{
public:
    CAbilitySpellSlotMeta();

    unsigned int m_count;  // +0x00 - 11

    struct Entry
    {
        int b;  // +0x00 (of the entry) - -1
        int a;  // +0x04 - -1
        int c;  // +0x08 - 0
    };
    Entry m_slots[11];     // +0x04
};

#endif
