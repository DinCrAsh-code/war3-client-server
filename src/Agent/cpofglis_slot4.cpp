//============================================================================
//  0x6F4A88F0 - NIpse::CPoFgLis::Slot4, vtable slot 4. Same single-`jmp`
//  tail call into CAgentRelation::Teardown as CPoLkLis::Slot4
//  (Agent/cpolklis_slot4.cpp).
//============================================================================
#include "cpofglis.h"
#include "agentrelation.h"

namespace NIpse {

void CPoFgLis::Slot4(int arg)
{
    ((CAgentRelation*)this)->Teardown(arg);
}

}  // namespace NIpse
