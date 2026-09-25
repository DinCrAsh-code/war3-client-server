//============================================================================
//  0x6F479D90 - NIpse::CPrBehavior::`scalar deleting destructor'(unsigned int),
//  vtable slot 2. BYTE-IDENTICAL (down to raw_bytes, apart from the call
//  displacement) to the other three degenerate scalar-deleting-destructors
//  this chain already has at slot 2 - 0x6F479910 (CPresence), 0x6F479C60
//  (CPresenceTagged), 0x6F47FFF0 (CPrRelation/CAgentRelation), all
//  Agent/presence.h's own file header documents in full. Named
//  `ScalarDeletingDestructorTrivial` for the same reason that file's own
//  copy is: no base teardown, no `this != 0` guard, restamps straight to
//  `??_7CPresence@NTempest@@6B@` regardless of which class's vtable this
//  was actually reached through.
//============================================================================
#include "prbehavior.h"
#include "storm.h"

namespace NIpse {

void* CPrBehavior::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
