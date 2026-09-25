//============================================================================
//  CAbilitySpell: the base every "real" usable spell (an ability with a
//  cost, a cooldown, and cast/channel timing) derives from - the
//  `CAbilitySpellBook` construction chain's own middle link
//  (CAbilityButton -> CAbilitySpell -> CAbilitySpellBook), and, by the
//  same RTTI naming this whole cluster was found through (live IDA
//  symbol search), the base of the hundreds of individual `CAbilityXxx`
//  spell classes this binary carries (`CAbilityDeathCoil`, `CAbilityHex`,
//  ...) - none of those per-spell overrides are reconstructed here, only
//  the shared base every one of them stands on.
//
//  Field layout from CAbilitySpell::CAbilitySpell's own writes
//  (0x6F068DF0 - which also carries CAbilityButton's own construction,
//  inlined, ahead of this class's own vtable re-stamp):
//    +0x80  m_cost        - FloatMini, g_CFloatZero (mana/gold cost, most
//                            likely - unconfirmed, nothing in this call
//                            tree reads it back)
//    +0x88  m_cooldown     - FloatMini, g_CFloatZero
//    +0x90  m_cooldownMax  - FloatMini, g_CFloatZero
//    +0x98  m_castTime     - FloatMini, g_CFloatZero
//    +0xA8  m_field0xA8    - 0
//    +0xA4  m_field0xA4    - -1
//    +0xA0  m_field0xA0    - -1
//    +0xAC  m_field0xAC    - 0
//    +0xB0  m_field0xB0    - FloatMini, g_CFloatZero (a fifth - order
//                            confirmed against the dump: written after
//                            the four raw dwords above, not adjacent to
//                            the other four FloatMinis)
//    +0xB8  m_field0xB8    - 0
//    +0xBC  m_cooldownTimer - CAgentTimer (agenttimer.h)
//    +0xD0  m_castTimer     - CAgentTimer
//  None of the four raw dwords or the fifth FloatMini's own purpose is
//  read by this call tree, so - CLAUDE.md's own rule - they are named by
//  position, not by guessed meaning; only m_cost/m_cooldown/m_cooldownMax/
//  m_castTime/m_cooldownTimer/m_castTimer carry a real-world guess, and
//  even those are inferred from the class's own name and WC3's known
//  spell model, not from anything this call tree reads back.
//============================================================================
#ifndef ABILITYSPELL_H
#define ABILITYSPELL_H

#include "abilitybutton.h"
#include "floatmini.h"

class CAbilitySpell : public CAbilityButton
{
public:
    //  0x6F068DF0 - see abilityspell.cpp.
    CAbilitySpell();

    //  0x6F0377F0 / 0x6F069020 - see abilityspell_reset.cpp. Trivial, but
    //  declared (not defined) here: neither has a call site anywhere in
    //  this call tree, and MSVC's /Ob2 discards an unreferenced
    //  header-inline member entirely rather than emitting a dead COMDAT
    //  for it - verify.py's own build showed exactly that ("not compiled
    //  into src/") until these moved out of line.
    void SetField0xAC(unsigned int value);
    unsigned int GetField0xAC() const;

    //  0x6F050390 - see abilityspell_reset.cpp.
    void ResetField0xAC();

    //  0x6F050320 - see abilityspell_reset.cpp.
    void Method_ResolveAndCommit(const void* ref);

    FloatMini    m_cost;             // +0x80
    FloatMini    m_cooldown;         // +0x88
    FloatMini    m_cooldownMax;      // +0x90
    FloatMini    m_castTime;         // +0x98
    unsigned int m_field0xA8;        // +0xA8
    unsigned int m_field0xA4;        // +0xA4
    unsigned int m_field0xA0;        // +0xA0
    unsigned int m_field0xAC;        // +0xAC
    FloatMini    m_field0xB0;        // +0xB0
    unsigned int m_field0xB8;        // +0xB8
    CAgentTimer  m_cooldownTimer;    // +0xBC
    CAgentTimer  m_castTimer;        // +0xD0
};

#endif
