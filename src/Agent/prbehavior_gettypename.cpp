//============================================================================
//  0x6F479D70 - NIpse::CPrBehavior::GetTypeName, vtable slot 0. Trivial, no
//  `this` read, `retn` with zero stack bytes - the exact same shape every
//  other GetTypeName in this chain already establishes (Agent/presence.h,
//  Agent/agentrelation.h, Pathfinding/crlagent.h).
//============================================================================
#include "prbehavior.h"

namespace NIpse {

const char* CPrBehavior::GetTypeName()
{
    return "PrBehavior";
}

}  // namespace NIpse
