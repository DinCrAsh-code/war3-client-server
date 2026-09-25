//============================================================================
//  0x6F48C370 - NIpse::CPoLkLis::`scalar deleting destructor'(unsigned int)
//  (named ScalarDeletingDestructorTrivial per Agent/presence.h's own naming
//  precedent), vtable slot 2. A fourth instance of the byte-identical
//  restamp-to-CPresence-vftable triplet Agent/presence.h's own file header
//  documents for CPresence/CPresenceTagged/CAgentRelation: no base teardown
//  call, no `this != 0` guard, restamps straight to
//  `??_7CPresence@NTempest@@6B@` (0x6F951CA0) regardless of which class's
//  vtable this was reached through.
//============================================================================
#include "cpolklis.h"
#include "storm.h"

namespace NIpse {

void* CPoLkLis::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
