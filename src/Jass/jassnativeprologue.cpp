//============================================================================
//  0x6F3B28F0 - the throttled progress tick every JASS native that can take
//  a while calls on the way in.  JASS_CreateUnit (jasscreateunit_native.cpp)
//  is what brought it into this repo; it is called first, unconditionally,
//  before the player handle is even resolved.
//
//  It is a rate limiter around one optional callback: if nobody has
//  installed one, or fewer than 100ms have gone by since the last call, it
//  does nothing at all.  That is what makes it safe to call on the way into
//  a native the map script may run thousands of times in a frame.
//
//  Its own translation unit rather than jasscreateunit_native.cpp's: the
//  shipped code has them at 0x6F3B28F0 and 0x6F3C5230, a megabyte apart and
//  plainly different modules, and they call each other for real.
//============================================================================
#include "jasscreateunit.h"

//  __stdcall, not __cdecl: kernel32 exports this as `_GetTickCount@0`, and
//  a plain declaration asks the linker for `_GetTickCount` instead - the
//  same note actionqueuerecord_notify.cpp already carries.
extern "C" unsigned int __stdcall GetTickCount();

//  dword_6FAB6694 / dword_6FAB6698 are *declared* in jasscreateunit.h and
//  deliberately not defined anywhere in src/.  They are the game's own
//  globals: build_mix.py writes a DATA row binding each to its real address
//  only for a symbol that comes back EXTRN, so defining either here would
//  give the .mix a private, zero-filled copy while the real, unhooked
//  shipped code kept reading and writing the originals - the callback would
//  read as null forever and this function would become a no-op.  That is
//  link_check.py's "game global defined here, so never bound" class, and it
//  is what this file did on its first draft.  See CLAUDE.md.
//
//  The callback takes one `float` and cleans its own argument: the shipped
//  call site pushes four bytes (`fldz` / `push ecx` to reserve / `fstp
//  [esp]` to fill) and does *not* follow the call with an `add esp, 4`, so
//  the callee cleaned it - __stdcall, not __cdecl.

void __fastcall JassNativePrologue()
{
    if (!g_progressTick)
        return;

    unsigned int now = GetTickCount();

    //  Unsigned, and strictly greater: the shipped test is `sub` then
    //  `cmp ecx, 64h` / `jbe`, so exactly 100ms still does nothing.  The
    //  subtraction is what makes a GetTickCount wrap harmless.
    if (now - g_progressTickLast <= 100)
        return;

    g_progressTickLast = now;
    g_progressTick(0.0f);
}
