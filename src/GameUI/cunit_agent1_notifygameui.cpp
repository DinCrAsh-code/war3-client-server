//============================================================================
//  0x6F333230 - a depth-1/2 CUnit-vtable-BFS leaf (cunit_agent1_worklist.md).
//  Fetches the game UI singleton (GetGameUI, GameUI/gameui.h - already
//  behind its own unreproducible-SEH-frame naked thunk, so this call is a
//  real out-of-line one either way) and, if it exists, forwards this
//  function's own two register arguments and its one stack argument into
//  one of its members (0x6F2F75C0).
//
//  0x6F2F75C0 was outside this batch's own worklist and stayed a thunk
//  here until a later CUnit-vtable-closure session (reaching it from a
//  different direction) gave it a real body - now GameUI/gameuinotifyleaf.cpp,
//  its own translation unit rather than defined in this file: the shipped
//  binary calls it out-of-line for real, and having both a real
//  NotifyGameUIIfPresent and a real Notify in one TU let /O2 inline the
//  latter straight into the former, losing the byte-for-byte match. The
//  first argument is genuine dead code at this call site (read by neither
//  this function nor Notify's own body), not a transcription gap.
//============================================================================
#include "gameui.h"

//  GameUI/gameuinotifyleaf.cpp - real body, out-of-line.
struct SGameUINotifyLeaf { void __thiscall Notify(int a, int b, int c); };

//  0x6F333230.  `retn 4`: a plain three-argument forwarder with no `this`
//  of its own beyond what it passes through - `a` (ecx) and `b` (edx) both
//  travel unchanged into the notify call alongside the one stack argument,
//  which is exactly the shape a `__fastcall` free function (not a member)
//  gives it.
void __fastcall NotifyGameUIIfPresent(int a, int b, int c)
{
    SGameUI* ui = GetGameUI(0, 0);
    if (ui != 0)
        ((SGameUINotifyLeaf*)ui)->Notify(a, b, c);
}
