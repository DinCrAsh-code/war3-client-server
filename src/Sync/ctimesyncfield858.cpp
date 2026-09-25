//============================================================================
//  0x6F496810 - GetTimeSyncField858.  Reached from Missile/
//  bhpoprojectile_attach.cpp's own pool-allocation lookup
//  (CBhPoProjectilePool::AllocateFromPool, 0x6F47AA10) as the receiver whose
//  own allocator lives at this offset.
//
//  Own translation unit, own module: 0x6F496810 is nowhere near
//  CSyncChannel.cpp's own 0x6F483xxx cluster.
//
//  A genuine free function: it reads the global `g_pTimeSync` directly
//  (`mov eax, dword_6FAB73D8`), takes no `this` and no stack argument at
//  all (`retn` with nothing to pop).  Declaring it as a member instead
//  would force every call site to load `ecx` for a receiver the body never
//  reads - LookupAndArmSlot's own call site (bhpoprojectile_attach.cpp)
//  does not, so this stays a plain function (game.h).
//============================================================================
#include "game.h"

void* GetTimeSyncField858()
{
    return (char*)g_pTimeSync + 0x858;
}
