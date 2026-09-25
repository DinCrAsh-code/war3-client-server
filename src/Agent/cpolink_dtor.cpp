//============================================================================
//  0x6F48D3E0 - NIpse::CPoLink::`scalar deleting destructor'(unsigned int),
//  vtable slot 2. Same byte-identical restamp-to-CPresence-vftable triplet
//  every class in this base chain shares.
//============================================================================
#include "cpolink.h"
#include "storm.h"

namespace NIpse {

void* CPoLink::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
