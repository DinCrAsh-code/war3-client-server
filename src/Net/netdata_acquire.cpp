//============================================================================
//  AcquireNetData (0x6F551940) - the generic "get-or-create slot `index` of
//  the per-thread game-data cache, construct a `SNetSessionInfo` there"
//  registry helper - the exact same shape GameUI/acquiregamesingleton.cpp
//  (CGameWar3, slot 6) and Net/randdataprovider.cpp (CRandData, slot 3)
//  already established, this time for the "net" checksum-provider slot
//  (index 2 at every real call site this session found -
//  docs/notes/checksum-provider-registry.md).  `retn 0` - the index arrives
//  in `ecx` and the unused second argument in `edx`, both `__fastcall`, and
//  the callee never reads the second one.
//
//  A real `__CxxFrameHandler3`-shaped SEH frame wraps the body (the
//  `SNetSessionInfo` constructor below can, in principle, run arbitrary
//  code) - this build's unreproducible ceiling, documented rather than
//  modelled (docs/msvc-vc8-idioms.md).
//
//  The allocation size is the literal `0x23C8` the dump pushes, not
//  `sizeof(SNetSessionInfo)` - that struct (netdata.h) only names the
//  fields this repo's other closures actually reach, so its C++ size is
//  smaller than the real shipped object; the same gap AcquireGameSingleton
//  already documents for `CGameWar3`/`0x408`.  The log-filename string is
//  this module's own, not Data.h's generic one - `worktree_store.py resolve`
//  gives the exact text.
//
//  `context->m_pCache` is re-read fresh from `context` twice - once to
//  store the freshly built object, once again to read the same slot back
//  for the return value - the same double-read AcquireGameSingleton's own
//  header already explains: `context` is an unconstrained pointer the
//  compiler cannot prove the constructor call did not just write through.
//============================================================================
#include "gamecontext.h"
#include "netdata.h"
#include "storm.h"

void* __fastcall AcquireNetData(int index, int /*unused*/)
{
    CGameDataContext* context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);

    void* block = SMemAlloc(0x23C8,
        "e:\\drive1\\temp\\buildwar3x\\war3\\source\\net\\../Data.h", 0x74, 0);

    void* obj = 0;
    if (block)
        obj = new (block) SNetSessionInfo();

    ((void**)context->m_pCache)[index] = obj;

    return ((void**)context->m_pCache)[index];
}
