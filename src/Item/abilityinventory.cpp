//============================================================================
//  CAbilityInventory::CAbilityInventory - see abilityinventory.h.
//============================================================================
#include "abilityinventory.h"

//----------------------------------------------------------------------------
//  0x6F0FC6C0
//----------------------------------------------------------------------------
//  A member-initializer list, same reasoning as CAbility::CAbility
//  (ability.cpp) - it is the six-element m_slots array's own real
//  constructor calls this needs unpinned from running before every POD
//  field store, matching the dump placing the array construction last.
CAbilityInventory::CAbilityInventory()
    : m_slotCount1(6),
      m_slotAux0a(-1), m_slotAux0b(-1), m_slotAux0c(0),
      m_slotAux1a(-1), m_slotAux1b(-1), m_slotAux1c(0),
      m_slotAux2a(-1), m_slotAux2b(-1), m_slotAux2c(0),
      m_slotAux3a(-1), m_slotAux3b(-1), m_slotAux3c(0),
      m_slotAux4a(-1), m_slotAux4b(-1), m_slotAux4c(0),
      m_slotAux5a(-1), m_slotAux5b(-1), m_slotAux5c(0),
      m_field0xB8(0), m_slotCount2(6),
      m_field0xC0(0), m_field0xC4(0), m_field0xC8(0),
      m_field0xCC(0), m_field0xD0(0), m_field0xD4(0),
      m_slotCapacity(6),
      m_field0x154(0)
{
}
