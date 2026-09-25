//============================================================================
//  0x6F48B3F0 - NIpse::CPoRePulseMod::`scalar deleting destructor'
//  (unsigned int), vtable slot 2. Same restamp-to-CPresence triplet.
//============================================================================
#include "cporepulsemod.h"
#include "storm.h"

namespace NIpse {

void* CPoRePulseMod::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
