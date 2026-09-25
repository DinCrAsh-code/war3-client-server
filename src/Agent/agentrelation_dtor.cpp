//============================================================================
//  0x6F47FFF0 - NIpse::CPrRelation::`scalar deleting destructor'(unsigned int)
//  (`CAgentRelation::ScalarDeletingDestructor`), vtable slot 2.
//  Byte-identical (per agent_worktrees/funcs/'s own raw_bytes) to
//  CPresence's and CPresenceTagged's own scalar deleting destructors apart
//  from address - see Agent/presence.h's own header comment for the full
//  oddity this trio shares: no base teardown call, no `this != 0` guard,
//  restamps straight to `??_7CPresence@NTempest@@6B@` rather than this
//  class's own vtable.
//============================================================================
#include "agentrelation.h"
#include "storm.h"

void* CAgentRelation::ScalarDeletingDestructor(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}
