//============================================================================
//  0x6F4AA040 - NIpse::CPoLkLis::Slot4, vtable slot 4. The entire shipped
//  body is a single `jmp` into CAgentRelation::Teardown (0x6F4A6920,
//  Agent/agentrelation.h) - argument count and `retn 4` both match, so this
//  is MSVC's own sibling-call tail-jmp optimisation for a call in tail
//  position with a matching argument list (docs/msvc-vc8-idioms.md's own
//  "MSVC's sibling-call optimisation" note), not a naked thunk: the callee
//  is this repo's own reconstruction, and the override genuinely replaces
//  the slot with the base's own teardown rather than adding new behaviour.
//============================================================================
#include "cpolklis.h"
#include "agentrelation.h"

namespace NIpse {

void CPoLkLis::Slot4(int arg)
{
    ((CAgentRelation*)this)->Teardown(arg);
}

}  // namespace NIpse
