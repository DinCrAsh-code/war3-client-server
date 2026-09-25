//============================================================================
//  0x6F484580 - NIpse::CPoSeparate::`scalar deleting destructor'(unsigned int),
//  vtable slot 2. Byte-identical to the whole chain's own no-base-teardown
//  restamp-to-CPresence triplet (Agent/presence.h's own header comment for
//  the full oddity write-up): no base destructor call, no `this != 0`
//  guard, restamps straight to `??_7CPresence@NTempest@@6B@` regardless of
//  CPoSeparate's own, much deeper position in the chain.
//============================================================================
#include "poseparate.h"
#include "storm.h"

namespace NIpse {

void* CPoSeparate::ScalarDeletingDestructor(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
