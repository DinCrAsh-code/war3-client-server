//============================================================================
//  TeardownNetDataSlot (0x6F5519D0) - the generic "destroy-and-clear slot
//  `index` of the per-thread game-data cache" registry helper, the mirror
//  image of AcquireNetData/AcquireGameSingleton/AcquireRandDataProvider's
//  own get-or-create shape (netdata_acquire.cpp, GameUI/
//  acquiregamesingleton.cpp, randdataprovider.cpp): `__fastcall`, index in
//  `ecx`, `retn 0`.  The one caller this session found is
//  TeardownNetSession (netdata_sessionteardown.cpp), tearing down index 2
//  (the "net" checksum-provider slot, `SNetSessionInfo`) during net-session
//  teardown - docs/notes/checksum-provider-registry.md.
//
//  Its own translation unit (rather than sharing one with
//  TeardownNetSession, which calls it) for the same reason
//  netdata_clearsessiontag.cpp is split out too: this build has no
//  whole-program optimisation, so a cross-TU call always stays a real
//  `call`, while two functions sharing a TU get inlined into each other at
//  `/O2` - CLAUDE.md's own "put a callee in its own TU rather than
//  `__declspec(noinline)`" rule.
//
//  Written generically (`void* obj` rather than `SNetSessionInfo*`) because
//  the shipped body itself is generic: it calls whatever destructor the
//  slot's own vtable-adjacent handling expects through a *fixed* address
//  (`sub_6F54FC50`, `SNetSessionInfo::~SNetSessionInfo`) rather than through
//  any stored function pointer - so this helper is only ever actually used
//  for the one slot its one real caller passes, and a second caller passing
//  a different index would call the wrong destructor.  That is the shipped
//  behaviour, not a modelling shortcut: `agent_worktrees` shows no other
//  call site, so nothing here contradicts it.
//
//  `context->m_pCache` is re-read fresh off `context` for each of the three
//  stores (one per `retn`), never reused across statements - the same
//  aliasing shape AcquireNetData's own header documents for its two reads,
//  and for the same reason: `context` is unconstrained, so the compiler
//  cannot carry `m_pCache` in a register across the destructor/free calls
//  in between.  The initial "is it live" test and the destructor's `this`
//  *do* share one fetch (`eax` in the dump), because nothing observable runs
//  between them.
//
//  Known remaining gap: the shipped body's three `retn`s each reload
//  `context->m_pCache` into a *different* register (`ecx`/`edx`/`eax`), so
//  they are three distinct instruction sequences the real compiler never
//  tail-merged; this build's optimiser folds the (textually identical)
//  `cache[index] = 0; return;` statement below into one shared block
//  instead, three calls into one physical epilogue rather than three. No
//  C++ spelling found so far stops that fold - flagged rather than
//  guessed around.
//
//  The unused second (`edx`) parameter reproduces the one real call site's
//  own `xor edx,edx` / `lea ecx,[edx+2]` - the same unused-edx-materialises-
//  first-then-`lea`-reuses-it shape AcquireNetData's own single real call
//  site already needs (netdata_acquire.cpp/netdata_initconfig.cpp): with
//  only one declared parameter the compiler has no reason to zero `edx`
//  before loading `ecx`, and emits a plain `mov ecx,2` instead.  The real
//  callee never reads `edx` either way (`retn 0`, only `ecx` used in its
//  own prologue), so this changes nothing about the ABI.
//============================================================================
#include "gamecontext.h"
#include "netdata.h"
#include "storm.h"

void __fastcall TeardownNetDataSlot(int index, int /*unused*/)
{
    CGameDataContext* context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);
    void** cache = (void**)context->m_pCache;

    if (cache[index] != 0)
    {
        void* obj = cache[index];
        if (obj != 0)
        {
            ((SNetSessionInfo*)obj)->~SNetSessionInfo();
            SMemFree(obj, "delete", -1, 0);
        }
        ((void**)context->m_pCache)[index] = 0;
        return;
    }

    ((void**)context->m_pCache)[index] = 0;
}
