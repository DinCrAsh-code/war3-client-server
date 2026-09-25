//============================================================================
//  0x6F0074F0 - AcquireGameSingleton: the generic "get-or-create slot N of
//  the per-thread game-data cache" registry helper.  `retn 0` - the index
//  arrives in ecx and the unused second argument in edx, both by
//  __fastcall, and the callee never reads the second one.
//
//  Real, unhooked callers all pass index 6 and land in
//  CGameDataContext::m_pCache[6], which is where CGameWar3 lives (the
//  "game" checksum-provider slot - docs/notes/checksum-provider-registry.md
//  - and the per-game player-slot table g_unk6FAB65F4 already read
//  throughout GameUI/, Agent/, Item/ and Widget/).  A real
//  __CxxFrameHandler3-shaped SEH frame wraps the body (this batch's
//  constructor call can throw); that frame is this build's unreproducible
//  ceiling, documented the same way as every other SEH-framed function in
//  this repo (docs/msvc-vc8-idioms.md).
//
//  `context->m_pCache` is re-read fresh from `context` twice - once to
//  store the freshly built object, once again to read the same slot back
//  for the return value - rather than kept in a register across the
//  CGameWar3 constructor call.  Written as plain member accesses this
//  toolchain reproduces that on its own, for the same reason
//  Unit/unitcreateparams.cpp's own globals are re-read: `context` is an
//  unconstrained pointer the compiler cannot prove the constructor call
//  did not just write through.
//============================================================================
#include "gamecontext.h"
#include "gamewar3.h"
#include "storm.h"

void* __fastcall AcquireGameSingleton(int index, int /*unused*/)
{
    CGameDataContext* context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);

    void* block = SMemAlloc(0x408, "e:\\drive1\\temp\\buildwar3x\\war3\\source\\Data.h", 0x74, 0);

    void* obj = 0;
    if (block)
        obj = new (block) CGameWar3();

    ((void**)context->m_pCache)[index] = obj;

    return ((void**)context->m_pCache)[index];
}
