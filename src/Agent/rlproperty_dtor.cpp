//============================================================================
//  0x6F4875C0 - NIpse::CRlProperty::ScalarDeletingDestructorTrivial, vtable
//  slot 2.  Byte-identical (down to raw_bytes, apart from the call
//  displacement) to CPresence's/CPresenceTagged's/CPrRelation's own scalar
//  deleting destructors - see Agent/presence.h's own header comment for the
//  full byte-identical-triplet oddity this is now a fifth instance of: no
//  base teardown call, no `this != 0` guard, restamps straight to
//  `??_7CPresence@NTempest@@6B@` regardless of this class's own vtable.
//============================================================================
#include "rlproperty.h"
#include "storm.h"

namespace NIpse {

void* CRlProperty::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
