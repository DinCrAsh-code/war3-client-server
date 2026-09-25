//============================================================================
//  0x6F468C60 / 0x6F4858F0 - one IDA function split into two chunks (both
//  captured together under 0x6F468C60's own agent_worktrees raw_asm): a
//  thin thunk that loads `ecx = g_pTimeSync` (`dword_6FAB73D8` -
//  Game/game.h, a completely different global from the presence host) and
//  falls into a shared body that walks BOTH of the tempest presence host's
//  own free-list pools (the positive one at `g_unk6FAB778C` itself, the
//  negative one at `g_unk6FAB778C+0x20` - the exact STwoIndexedFreeLists
//  layout Math/indexedfreelist.h already establishes) and, for every
//  populated entry in each, calls that entry's own object through vtable
//  slot 14 (`+0x38`, this-call, no extra argument) - a plain per-entry
//  broadcast, not the accumulator-taking slot 15 (`+0x3C`) hash callback
//  Net/tempestproviderlisthash.cpp's TempestProviderListHash already
//  reconstructed.  `ecx` (g_pTimeSync) is loaded into the shared body's own
//  `esi` and preserved across both calls, but never actually read by
//  either - this looks like a non-virtual body shared by more than one
//  class's own vtable slot, of which this instantiation (`g_pTimeSync`'s
//  own slot) happens not to need `this` for anything beyond the call.
//
//  Investigated as part of "does any of the 8 unreconstructed
//  g_unk6FAB778C-touching functions perform the missing
//  STwoIndexedFreeLists::AllocateAt call": no - this only *reads* already-
//  populated entries and broadcasts a notification-style call into each;
//  it never stores anything into either pool.
//
//  The one static caller found (`sub_6F3AFE90`) is a large, ~0x8A4-byte-
//  frame game/map-start bring-up function (it also acquires `CGameWar3`
//  through `AcquireGameSingleton` a few instructions later, the same
//  "game" checksum-provider singleton `docs/notes/checksum-provider-
//  registry.md` already traces) - this runs once per map load, not per
//  tick and not on a unit-movement path.
//
//  `PoolBroadcastNotify` (the walker each pool call reaches, matching
//  `SIndexedFreeListOwner`'s own `+0x0C` records / `+0x1C` capacity - the
//  exact fields Math/indexedfreelist.h already names) is not one of this
//  session's eight addresses, so it stays a naked redirect here. Its own
//  body always returns 1 regardless of any entry's own callback result (a
//  `mov eax, 1` right before every return path, including the
//  zero-capacity early-out) - so the boolean-normalising code around each
//  call in the shared body (`neg`/`sbb`/`neg`) is boilerplate that never
//  actually observes a 0 in this shipped instantiation, and this function
//  always returns nonzero.
//============================================================================
#include "tempestpresencehost.h"

//  0x6F485110 - `retn 4` (one stack argument: the pool base, either
//  `g_unk6FAB778C` itself or `g_unk6FAB778C+0x20`).  Walks
//  `pool->m_records[0..pool->m_capacity)` (the SIndexedFreeListOwner shape,
//  Math/indexedfreelist.h), and for every record whose own tag dword is
//  exactly 0xFFFFFFFE (the same "populated" tag TempestProviderListHash
//  checks) with a non-null payload, calls the payload's own vtable slot 14
//  (offset 0x38, this-call, no stack argument).  Always returns 1.
ADDR_THUNK(int __stdcall PoolBroadcastNotify(void* pool), 0x6F485110)

//  The real thunk's own `this` (g_pTimeSync, dword_6FAB73D8, Game/game.h) is
//  not modelled as a parameter here - it is loaded into a register and
//  preserved across both calls below but never actually read by either, so
//  omitting it changes no observable behaviour.  Kept out rather than
//  pulling in game.h's own CTimeSync* declaration for an argument nothing
//  in this function's own body uses.
int BroadcastNotifyTempestPresenceEntries()
{
    if (!PoolBroadcastNotify(g_unk6FAB778C))
        return 0;

    return !!PoolBroadcastNotify((char*)g_unk6FAB778C + 0x20);
}
