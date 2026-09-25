//============================================================================
//  CAbilitySpellBook::CAbilitySpellBook - see abilityspellbook.h.
//============================================================================
#include "abilityspellbook.h"

//----------------------------------------------------------------------------
//  0x6F1F7400
//----------------------------------------------------------------------------
CAbilitySpellBook::CAbilitySpellBook()
    : m_slotCount(11)
{
    m_handles[0] = 0;
    m_handles[1] = 0;
    m_handles[2] = 0;
    m_handles[3] = 0;
    m_handles[4] = 0;
    m_handles[5] = 0;
    m_handles[6] = 0;
    m_handles[7] = 0;
    m_handles[8] = 0;
    m_handles[9] = 0;
    m_handles[10] = 0;

    m_field0x19C = 0;
}
