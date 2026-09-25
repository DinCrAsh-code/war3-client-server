//============================================================================
//  0x6F4A88E0 - NIpse::CPoFgPulseMod::Slot4, vtable slot 4. Bare `jmp`
//  into CAgentRelation::Teardown - same shape as
//  Agent/cpolklis_slot4.cpp's own CPoLkLis::Slot4.
//============================================================================
#include "cpofgpulsemod.h"
#include "agentrelation.h"

namespace NIpse {

void CPoFgPulseMod::Slot4(int arg)
{
    ((CAgentRelation*)this)->Teardown(arg);
}

}  // namespace NIpse
