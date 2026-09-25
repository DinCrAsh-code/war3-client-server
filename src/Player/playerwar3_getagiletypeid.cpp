//============================================================================
//  0x6F40AFD0 - CPlayerWar3::GetAgileTypeId, vtable slot 7 (+0x1C).  A bare
//  `mov eax, imm32`, same shape as CAgentWar3::GetAgileTypeId
//  (agentwar3_slots.cpp) and its siblings: the FourCC is stored with its
//  first printed character in the low byte, so 0x2B706C79 reads back as
//  '+','p','l','y' - "+ply".
//============================================================================
#include "playerwar3.h"

unsigned int CPlayerWar3::GetAgileTypeId() const
{
    return 0x2B706C79;
}
