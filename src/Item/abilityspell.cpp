//============================================================================
//  CAbilitySpell::CAbilitySpell - see abilityspell.h.  A member-
//  initializer list with an empty body, same reasoning as
//  CAbility::CAbility (ability.cpp) - the two embedded CAgentTimer
//  members need to be unpinned from running before the POD field stores
//  to match the dump's own scheduling (both timers constructed last).
//============================================================================
#include "abilityspell.h"

//----------------------------------------------------------------------------
//  0x6F068DF0 (also carries CAbilityButton::CAbilityButton, inlined - see
//  abilitybutton.h)
//----------------------------------------------------------------------------
CAbilitySpell::CAbilitySpell()
    : m_field0xA8(0), m_field0xA4(0xFFFFFFFF), m_field0xA0(0xFFFFFFFF),
      m_field0xAC(0), m_field0xB8(0)
{
}
