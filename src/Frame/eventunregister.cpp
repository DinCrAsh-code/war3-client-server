//============================================================================
//  0x6F62B430 - UnregisterEventHandler: RegisterEventHandler's opposite
//  number, and the same three steps in the same order - this thread's own
//  event table out of thread-local slot 0, the registry stripe that table
//  hashes to, the work, and the stripe signalled again on the way out.
//
//  Unlike its twin it does no parameter validation of its own: the 0x57 for
//  a null table is RemoveEventHandlers'.
//============================================================================
//  The stripe is signalled with the flag AcquireBucket published, and that
//  flag is `lockIndex >= 8`, not `< 8`: AcquireBucket writes
//  `stripe + (exclusive ? 8 : 0)`, so the high bit of the published index
//  *is* the exclusive flag it was taken with.  The shipped tail spells it
//  `cmp 8` / `sbb` / `add 1`, which is 0 below eight and 1 at or above -
//  the opposite of what this file used to say, and a real behavioural
//  difference rather than a spelling one.
//============================================================================
#include "frame.h"
#include "framethunks.h"

#include "gamecontext.h"

void __fastcall UnregisterEventHandler(int slot, void* handler, void* a,
                                       int flags)
{
    int lockIndex;
    void* table = GetThreadLocalSlot(0);

    CEventBucketList* list = (CEventBucketList*)g_eventRegistry.AcquireBucket(
        table, 0, &lockIndex, 0, 1);

    if (list)
    {
        RemoveEventHandlers(list, slot, handler, (int)a, flags);
        if (lockIndex != -1)
            EventRegistryLock(lockIndex & 7)->SignalOne(
                (unsigned int)lockIndex >= 8);
    }
}
