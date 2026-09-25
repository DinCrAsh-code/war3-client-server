//============================================================================
//  0x6F4A8CC0 - NIpse::CPoFgLis::GetEndpointBRaw, own translation unit. One
//  instruction: `this`'s own endpoint B (CAgentRelation::m_endpointB,
//  Agent/agentrelation.h, +0x40) - a second, separately-compiled copy of
//  the identical accessor CDrivenRelation::GetTrace (0x6F4A9A50,
//  agentrelation.h) already reconstructs at a different address. See
//  cpofglis.h's own declaration comment.
//============================================================================
#include "cpofglis.h"
#include "agentrelation.h"

namespace NIpse {

void* CPoFgLis::GetEndpointBRaw()
{
    return ((CAgentRelation*)this)->m_endpointB;
}

}  // namespace NIpse
