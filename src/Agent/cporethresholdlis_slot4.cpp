//============================================================================
//  0x6F4A8F50 - NIpse::CPoReThresholdLis::Slot4, vtable slot 4. Bare
//  `jmp` into CAgentRelation::Teardown.
//============================================================================
#include "cporethresholdlis.h"
#include "agentrelation.h"

namespace NIpse {

void CPoReThresholdLis::Slot4(int arg)
{
    ((CAgentRelation*)this)->Teardown(arg);
}

}  // namespace NIpse
