//============================================================================
//  0x6F4A8F40 - NIpse::CPoRePulseMod::Slot4, vtable slot 4. Bare `jmp`
//  into CAgentRelation::Teardown.
//============================================================================
#include "cporepulsemod.h"
#include "agentrelation.h"

namespace NIpse {

void CPoRePulseMod::Slot4(int arg)
{
    ((CAgentRelation*)this)->Teardown(arg);
}

}  // namespace NIpse
