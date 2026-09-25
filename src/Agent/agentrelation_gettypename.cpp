//============================================================================
//  0x6F47FFD0 - NIpse::CPrRelation::GetTypeName (`CAgentRelation::GetTypeName`
//  under this codebase's own established alias - see agentrelation.h's own
//  file header), vtable slot 0.  Trivial, no `this` read, `retn` with zero
//  stack bytes - same shape as Pathfinding/crlagent_gettypename.cpp's own
//  `CRlAgent_GetTypeName` and Agent/presence.h's own
//  `CPresence::GetTypeName`/`CPresenceTagged::GetTypeName`.
//============================================================================
#include "agentrelation.h"

const char* CAgentRelation::GetTypeName()
{
    return "PrRelation";
}
