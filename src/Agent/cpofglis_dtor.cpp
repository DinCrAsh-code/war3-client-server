//============================================================================
//  0x6F48BE80 - NIpse::CPoFgLis::ScalarDeletingDestructorTrivial, vtable
//  slot 2. A fifth instance of the byte-identical restamp-to-CPresence-
//  vftable triplet Agent/presence.h's own file header documents.
//============================================================================
#include "cpofglis.h"
#include "storm.h"

namespace NIpse {

void* CPoFgLis::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
