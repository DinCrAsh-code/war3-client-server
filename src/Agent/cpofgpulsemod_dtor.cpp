//============================================================================
//  0x6F48BC90 - NIpse::CPoFgPulseMod::`scalar deleting destructor'
//  (unsigned int), vtable slot 2. Same restamp-to-CPresence triplet.
//============================================================================
#include "cpofgpulsemod.h"
#include "storm.h"

namespace NIpse {

void* CPoFgPulseMod::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
