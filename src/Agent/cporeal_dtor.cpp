//============================================================================
//  0x6F48C6B0 - NIpse::CPoReal::`scalar deleting destructor'(unsigned int),
//  vtable slot 2. Same restamp-to-CPresence triplet as the rest of this
//  family.
//============================================================================
#include "cporeal.h"
#include "storm.h"

namespace NIpse {

void* CPoReal::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
