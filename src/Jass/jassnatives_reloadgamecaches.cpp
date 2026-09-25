//============================================================================
//  0x6F3BC430 - JASS's `ReloadGameCachesFromDisk ()B`.
//
//  Fifteen instructions across two chunks IDA gives one heading: the body,
//  and the `push 1` / `call` tail at 0x6F406430 the body tail-jumps to.  The
//  world object is read *before* the gate is tested - the shipped code loads
//  it into esi first - which is why it is a named local here rather than an
//  expression inside the `if`.
//
//  See docs/targets/JASS_ReloadGame.md.
//============================================================================
#include "gamerestart.h"

int __cdecl JASS_ReloadGameCachesFromDisk()
{
    void* world = g_unk6FAB65F4;

    if (!ReloadGateOpen())
        return 0;

    return ((SGameCacheStore*)WorldEnsureObjectDataCache(world))
               ->ReloadFromDisk(1);
}
