//============================================================================
//  0x6F4722D0 - CAgentCantPathEvent::`scalar deleting destructor'(unsigned
//  int), vtable slot 1.  Same byte-identical-triplet shape
//  Agent/presence.h's own file header documents for CPresence's
//  descendants - no base teardown, no `this != 0` guard - just restamped
//  one level further up the chain, to `??_7TRefCnt@@6B@` rather than to
//  `??_7CPresence@NTempest@@6B@`, because this class's own base (CEvent)
//  has nothing non-trivial of its own to fold away either.
//============================================================================
#include "agentcantpathevent.h"
#include "storm.h"

void* CAgentCantPathEvent::ScalarDeletingDestructor(unsigned int flags)
{
    *(void**)this = (void*)0x6F8765F8;   // ??_7TRefCnt@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}
