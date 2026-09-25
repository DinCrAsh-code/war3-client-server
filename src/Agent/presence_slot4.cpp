//============================================================================
//  0x6F4798C0 - NTempest::CPresence::Slot4, vtable slot 4.  See presence.h's
//  own comment: a hand-written indirect dispatch through `this`'s own
//  vtable slot 1 (`ReturnToStaticPool`), which every override in this chain
//  declares with zero stack arguments (agent_worktrees/funcs/ shows a bare
//  `retn` for every one of CPresence's/CPresenceTagged's/CPrRelation's/
//  CAgentBaseAbs's own slot-1 bodies) - so `flags` is deliberately never
//  forwarded to it.  No committed docs/targets/vtables/CPresence.txt exists
//  yet for tools/vtable_dispatch_audit.py to judge this site against.
//============================================================================
#include "presence.h"

namespace NTempest {

typedef void* (__thiscall *ReturnToPoolFn)(void*);

void* CPresence::Slot4(unsigned int /*flags*/)
{
    return ((ReturnToPoolFn)(*(void***)this)[1])(this);
}

}  // namespace NTempest
