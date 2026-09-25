//============================================================================
//  0x6F4A6C90 - NIpse::CPoPoVelocityMod::Slot4, vtable slot 4. Bare `jmp`
//  into CAgentRelation::Teardown.
//============================================================================
#include "cpopovelocitymod.h"
#include "agentrelation.h"

namespace NIpse {

void CPoPoVelocityMod::Slot4(int arg)
{
    ((CAgentRelation*)this)->Teardown(arg);
}

}  // namespace NIpse
