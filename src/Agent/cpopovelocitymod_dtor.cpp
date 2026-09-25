//============================================================================
//  0x6F48B920 - NIpse::CPoPoVelocityMod::`scalar deleting destructor'
//  (unsigned int), vtable slot 2. Same restamp-to-CPresence triplet shape
//  as most of this family (Agent/presence.h's own header) - unlike its
//  sibling CPoPoInterfLis, this class has no member needing a real
//  destructor call first.
//============================================================================
#include "cpopovelocitymod.h"
#include "storm.h"

namespace NIpse {

void* CPoPoVelocityMod::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
