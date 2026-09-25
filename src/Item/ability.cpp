//============================================================================
//  CAbility::CAbility - see ability.h.
//============================================================================
#include "ability.h"

//----------------------------------------------------------------------------
//  0x6F052190
//----------------------------------------------------------------------------
//  A member-initializer list for CAbility's own fields, with the one
//  inherited write (CAgentWar3::m_flags, unitreflist.h/agentwar3.h - not
//  CAbility's own member, so it cannot appear in this class's own
//  initializer list) left as a body statement instead. Same reasoning as
//  before for the scheduling freedom this needs: the embedded CAgentTimer
//  member's own out-of-line constructor call has to land last, after
//  every POD field store including this inherited one - matching item.h's
//  own constructor-inlining note for CItem's two embedded CAgentTimer
//  members.
CAbility::CAbility()
{
    m_flags = 0;
    m_field2C = 0;
    m_field30 = 0;
    m_typeId = 0;
    m_field38 = 0;
    m_field3C = 0;
    m_field40 = 0;
    m_field44 = 0;
    m_field48 = 0;
    m_field4C = 0;
    m_field50 = 0;
    m_field54 = 0;
}
