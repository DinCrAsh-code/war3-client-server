//============================================================================
//  0x6F041700/0x6F041720 - worklist batch E, slots 185/186:
//  IncrementLevel/DecrementLevel.  A matched pair, one shipped module
//  (0x6F0417xx) apart from ability_slots.cpp's own 0x6F02Exx/0x6F0522xx.
//
//  Each bumps m_field50 - CAbility's own "current level" (SetTypeId's own
//  refresh already reads it, ability_slots.cpp) - by one in its own
//  direction, then refreshes the level-row cache at m_field4C by calling
//  CAbility::QueryLevelRowRaw (0x6F02E6A0, ability_slots.cpp) with the
//  new level - the exact same refresh SetTypeId's own tail performs.
//  Neither reads the call's return value through any other path; both
//  just store it.  QueryLevelRowRaw was a naked thunk when this file was
//  first written; it is a real member now (this batch's own depth<=5
//  closure verification pass), so both callers just call it directly.
//============================================================================
#include "ability.h"

//----------------------------------------------------------------------------
//  0x6F041700 - slot 185, IncrementLevel.
//----------------------------------------------------------------------------
void CAbility::IncrementLevel()
{
    m_field50 += 1;
    m_field4C = QueryLevelRowRaw((int)m_field50);
}

//----------------------------------------------------------------------------
//  0x6F041720 - slot 186, DecrementLevel.
//----------------------------------------------------------------------------
void CAbility::DecrementLevel()
{
    m_field50 -= 1;
    m_field4C = QueryLevelRowRaw((int)m_field50);
}
