//============================================================================
//  0x6F62B170 - IsEventKeyRegistered: is `key` in this thread's own event
//  table?
//
//  The table itself is thread-local (slot 0 of the per-thread pointer array,
//  Game/gamecontext.h) and is looked up through the registry, whose stripe
//  lock stays held for exactly as long as the membership test takes.
//
//  The redirect the JASS_PauseGame slice stood in for this address under the
//  name LookupUiResource is gone: the body answers a membership question and
//  returns Storm's own 0/1, not a resource.
//============================================================================
#include "frame.h"
#include "framethunks.h"

#include "gamecontext.h"

//  0x6F6323E0 - Misc/lockedkeylist.cpp owns the body and the name.
//  Re-declared here against a local copy of the class, which mangles the same
//  way because MSVC puts only the class *name* in a member\'s mangled name.
class CLockedKeyTable
{
public:
    int __fastcall ContainsKey(void* key);
};


int __fastcall IsEventKeyRegistered(void* key)
{
    int found = 0;
    int lockIndex;

    //  The thread-local lookup is its own statement because the shipped code
    //  calls it before it pushes any of AcquireBucket's arguments; folded
    //  into the call it is scheduled after the first four pushes instead.
    void* context = GetThreadLocalSlot(0);

    CLockedKeyTable* table = (CLockedKeyTable*)g_eventRegistry.AcquireBucket(
        context, 0, &lockIndex, 0, 1);

    if (table)
    {
        found = table->ContainsKey(key);
        //  Both halves of this flag are read off the shipped code, not
        //  guessed: `cmp eax,8 / sbb edx,edx / add edx,1` is 1 when the index
        //  is *not* below 8 (sbb leaves -1 on borrow, and the add turns that
        //  into 0), and the borrow makes the comparison unsigned.  Spelling
        //  it `lockIndex < 8` gives the opposite answer, and spelling it
        //  signed gives `setl`, which disagrees for any negative index other
        //  than the -1 already excluded above.
        if (lockIndex != -1)
            EventRegistryLock(lockIndex & 7)->SignalOne((unsigned int)lockIndex >= 8u);
    }

    return found;
}
