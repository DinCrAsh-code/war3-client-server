//============================================================================
//  0x6F4AAC00 - GetTimeSyncField838. Reached from
//  Missile/bhpobehavior_typename_dtor.cpp's own real destructor override as
//  the receiver whose allocator lives at this offset - the `CBhPoBehavior`
//  sibling of `GetTimeSyncField858` (Sync/ctimesyncfield858.cpp), same
//  global, different field offset (+0x838 vs +0x858), so almost certainly
//  the same `g_pTimeSync`-owned array of per-class pool objects.
//
//  Own translation unit, own module, for the same reason
//  ctimesyncfield858.cpp's own header comment gives: nowhere near
//  CSyncChannel.cpp's own 0x6F483xxx cluster, and a genuine free function -
//  no `this`, no stack argument, `retn` with nothing to pop.
//============================================================================
#include "game.h"

void* GetTimeSyncField838()
{
    return (char*)g_pTimeSync + 0x838;
}
