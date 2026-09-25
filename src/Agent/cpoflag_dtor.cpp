//============================================================================
//  0x6F48D190 - NIpse::CPoFlag::`scalar deleting destructor'(unsigned int),
//  vtable slot 2. Same byte-identical restamp-to-CPresence-vftable triplet
//  every class in this base chain shares (Agent/presence.h's own file
//  header) - no base teardown, no `this != 0` guard.
//============================================================================
#include "cpoflag.h"
#include "storm.h"

namespace NIpse {

void* CPoFlag::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
