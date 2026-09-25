//============================================================================
//  0x6F479910 - NTempest::CPresence::`scalar deleting destructor'(unsigned int),
//  vtable slot 2.  Named `ScalarDeletingDestructorTrivial` - see
//  presence.h's own header comment for why the plain name is already
//  taken by a different address (0x6F4A3760, cpresence_dtor.cpp).  See
//  presence.h's own file header for the byte-identical-triplet oddity this
//  shares with CPresenceTagged's and CPrRelation's own: no base teardown
//  call, no `this != 0` guard, restamps straight to CPresence's own vtable
//  regardless of which of the three classes' vtable this was reached
//  through.
//============================================================================
#include "presence.h"
#include "storm.h"

namespace NTempest {

void* CPresence::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NTempest
