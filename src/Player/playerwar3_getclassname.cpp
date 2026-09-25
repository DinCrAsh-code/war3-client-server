//============================================================================
//  0x6F40AF80 - CPlayerWar3::GetClassName, vtable slot 22 (+0x58).  A bare
//  `mov eax, offset "CPlayerWar3"`, same shape as CAgentWar3::GetClassName
//  (agentwar3_slots.cpp) - confirms the class's own real name.
//============================================================================
#include "playerwar3.h"

const char* CPlayerWar3::GetClassName()
{
    return "CPlayerWar3";
}
