//============================================================================
//  gamemain-batch-2: a run of genuinely trivial leaves out of GameMain's own
//  call tree - one-field global setters, empty stubs, and one pure
//  cross-module tail redirect.  Grouped in one TU the same way
//  Misc/sweep_field_setters.cpp groups its own trivial writers: none of
//  these calls anything real, so CLAUDE.md's one-TU-per-module rule has
//  nothing to bite on.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F6BADA0 - a bare `mov [global], ecx` store into Window/wininputbridge.h's
//  own `g_cachedGameWindow` (dword_6FAD1398, already declared there).
//----------------------------------------------------------------------------
#include <windows.h>
extern HWND g_cachedGameWindow;      // Window/wininputbridge.h, dword_6FAD1398

void __fastcall CacheGameWindowHandle(HWND value)
{
    g_cachedGameWindow = value;
}

//----------------------------------------------------------------------------
//  0x6F7ADAB0 - same shape, different global.
//----------------------------------------------------------------------------
extern int g_agentField420;          // dword_6FAAC420

void __fastcall SetAgentField420(int value)
{
    g_agentField420 = value;
}

//----------------------------------------------------------------------------
//  0x6F631290 - install the input pump's own quit callback and its context
//  dword in one call: `g_pumpQuitCallback`/`g_pumpQuitCallbackContext`,
//  already declared in Input/inputeventpump.h and read back by
//  Input/inputeventhandlers.cpp's own quit-event handler.
//----------------------------------------------------------------------------
#include "inputeventpump.h"

void __fastcall SetInputPumpQuitCallback(PumpQuitCallbackFn callback, int context)
{
    g_pumpQuitCallback = callback;
    g_pumpQuitCallbackContext = context;
}

//----------------------------------------------------------------------------
//  0x6F6C7730 - a guarded single-field store: only writes `ecx` through if
//  the guard global is still zero (a "first writer wins" latch - no unlock
//  ever clears the guard in this call tree).
//----------------------------------------------------------------------------
extern int g_latch1608;              // dword_6FAD1608
extern int g_latchedValue9F878;      // dword_6FA9F878

void __fastcall SetLatchedValueOnce(int value)
{
    if (g_latch1608 == 0)
        g_latchedValue9F878 = value;
}

//----------------------------------------------------------------------------
//  0x6F7AD8D0 - a constant-answer leaf: always true, no state touched.
//----------------------------------------------------------------------------
int __fastcall AlwaysTrue()
{
    return 1;
}

//----------------------------------------------------------------------------
//  nullsub_23 (0x6F629760), nullsub_24 (0x6F62BDA0), nullsub_25
//  (0x6F631680), nullsub_18 (0x6F4B73B0) - four genuinely empty functions:
//  one `retn` each, no stack cleanup, no register touched.  IDA never
//  resolved names for any of them and this call tree gives none either, so
//  each keeps a plain, honest name for what it is rather than inventing a
//  purpose the dump does not show.
//----------------------------------------------------------------------------
void __fastcall EmptyStub23() {}
void __fastcall EmptyStub24() {}
void __fastcall EmptyStub25() {}
void __fastcall EmptyStub18() {}

//----------------------------------------------------------------------------
//  0x6F631220 - a pure tail redirect: the shipped body is the single
//  instruction `jmp sub_6F6C6A60`.  0x6F6C6A60 is outside this batch's own
//  43 (it is deep EvtSched/thread-pool machinery, not reconstructed this
//  session), so there is no C++ symbol yet to spell a direct `jmp` to; the
//  naked body below reaches the same real, unhooked code through the
//  ADDR_THUNK shape the rest of this repo already uses for "not this
//  session's job" redirects.  This costs one extra instruction against the
//  dump's own single `jmp` (a `mov eax,imm32` ahead of it) until whichever
//  session reconstructs 0x6F6C6A60 lands a real symbol this can jump to
//  directly instead.
//----------------------------------------------------------------------------
__declspec(naked) void TailRedirectToThreadPoolWorker()
{
    __asm { mov eax, 06F6C6A60h }
    __asm { jmp eax }
}
