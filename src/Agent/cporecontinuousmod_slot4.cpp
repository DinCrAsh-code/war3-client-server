//============================================================================
//  0x6F4A8EB0 - NIpse::CPoReContinuousMod::Slot4, vtable slot 4. Bare
//  `jmp` into CAgentRelation::Teardown - MSVC's own sibling-call
//  optimisation for a matching tail call, same shape as
//  Agent/cpolklis_slot4.cpp's own CPoLkLis::Slot4.
//============================================================================
#include "cporecontinuousmod.h"
#include "agentrelation.h"

namespace NIpse {

void CPoReContinuousMod::Slot4(int arg)
{
    ((CAgentRelation*)this)->Teardown(arg);
}

}  // namespace NIpse
