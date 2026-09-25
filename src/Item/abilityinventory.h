//============================================================================
//  CAbilityInventory: the ability WC3 attaches to any unit with item slots
//  (the object-editor "Inventory" ability - `??_7CAbilityInventory@@6B@`,
//  live IDA symbol search) - the answer to "where does a picked-up item's
//  data live": not on the item, but in this ability's own 6-slot array,
//  attached to the holding unit.
//
//  Derives from CAbility (ability.h) - CAbilityInventory::CAbilityInventory
//  (0x6F0FC6C0) calls CAbility::CAbility (0x6F052190) first, then stamps
//  its own vftable and adds its own fields starting at CAbility's own end
//  (+0x6C).
//
//  Field layout from CAbilityInventory::CAbilityInventory's own writes:
//    +0x6C  m_slotCount1     - 6 (the slot count, three times over -
//                               +0x6C/+0xBC/+0xD8 - once per sub-block
//                               below; not read back by this call tree,
//                               so which one is "the" live count, if any
//                               differ from a real one, is unconfirmed)
//    +0x70..+0xB7  a 6-element, 12-byte-stride array of plain (not
//                  object-constructed - a literal-only fill, no call)
//                  triples {-1, -1, 0}; nothing in this call tree reads
//                  it back, so its per-field meaning is not named beyond
//                  "parallel per-slot bookkeeping" alongside m_slots below
//    +0xB8  m_field0xB8      - 0
//    +0xBC  m_slotCount2     - 6
//    +0xC0..+0xD4  six zeroed dwords
//    +0xD8  m_slotCapacity   - 6 - the count argument to the array
//                               constructor immediately below
//    +0xDC  m_slots[6]       - the item slots themselves: six embedded
//                               CAgentTimer objects (agenttimer.h),
//                               object-constructed through the compiler's
//                               `eh vector constructor iterator` (the
//                               exception-safety wrapper - if slot N's
//                               constructor throws, slots 0..N-1 get
//                               unwound through CAgentTimer's own
//                               destructor, 0x6F477E20).  Each slot's own
//                               CAgentTimer::m_target (a CAgentPtr) is
//                               what actually holds the reference to the
//                               item occupying that slot - see
//                               agenttimer.h.
//    +0x154 m_field0x154     - 0, the field immediately after the array;
//                               not read by this call tree (a selected-
//                               slot index or in-use count are the two
//                               obvious guesses, neither confirmed)
//============================================================================
#ifndef ABILITYINVENTORY_H
#define ABILITYINVENTORY_H

#include "ability.h"

class CAbilityInventory : public CAbility
{
public:
    //  0x6F0FC6C0 - see abilityinventory.cpp.
    CAbilityInventory();

    unsigned int m_slotCount1;   // +0x6C

    //  +0x70..+0xB7 - six {-1,-1,0} triples, plain literal fill.
    //
    //  **Declaration order here is the layout, and must stay the layout.**
    //  These were once declared out of order (every pair's `b` ahead of its
    //  `a`, then all six `c`s together at the end) to try to drive the
    //  member-initialization order towards the dump's own store order.  That
    //  cannot work, and it is not a cosmetic mistake: declaration order in
    //  C++ fixes *both* the initialization order and the offsets, so
    //  reordering the declarations moved the fields instead of the stores.
    //  The struct came out the right size, every following offset still
    //  lined up, `verify.py` scored it 25/73 as usual and the write-up
    //  called the remainder codegen - but eight of these eighteen words were
    //  being given the other one's value: -1 landed at +0x78/+0x84/+0x90/
    //  +0x9C where the shipped code writes 0, and 0 landed at +0xA0/+0xA4/
    //  +0xAC/+0xB0 where it writes -1.  Live, that hands the inventory six
    //  slots whose bookkeeping words are half swapped, and the game crashed
    //  on it.
    //
    //  The dump's store order (all twelve -1s first, in b-then-a order per
    //  pair, then all six 0s) is MSVC scheduling the stores by which
    //  register holds the constant - eax=-1, edi=0 - not evidence about
    //  either declaration or initializer order.  It is not source-
    //  addressable; the offsets are.  Leave these in layout order.
    int m_slotAux0a, m_slotAux0b, m_slotAux0c;  // +0x70,+0x74,+0x78
    int m_slotAux1a, m_slotAux1b, m_slotAux1c;  // +0x7C,+0x80,+0x84
    int m_slotAux2a, m_slotAux2b, m_slotAux2c;  // +0x88,+0x8C,+0x90
    int m_slotAux3a, m_slotAux3b, m_slotAux3c;  // +0x94,+0x98,+0x9C
    int m_slotAux4a, m_slotAux4b, m_slotAux4c;  // +0xA0,+0xA4,+0xA8
    int m_slotAux5a, m_slotAux5b, m_slotAux5c;  // +0xAC,+0xB0,+0xB4

    unsigned int m_field0xB8;    // +0xB8
    unsigned int m_slotCount2;   // +0xBC

    unsigned int m_field0xC0;    // +0xC0
    unsigned int m_field0xC4;    // +0xC4
    unsigned int m_field0xC8;    // +0xC8
    unsigned int m_field0xCC;    // +0xCC
    unsigned int m_field0xD0;    // +0xD0
    unsigned int m_field0xD4;    // +0xD4

    unsigned int m_slotCapacity; // +0xD8

    CAgentTimer  m_slots[6];     // +0xDC - the item slots

    unsigned int m_field0x154;   // +0x154
};

#endif
