//============================================================================
//  0x6F001240 - ReleaseFourStorm252HandlesB3 (gamemain-batch-3).  Four
//  independent globals, each released through Storm ordinal 252 (not yet
//  identified by name in this repo - kept as `Storm_252`, the same
//  ordinal-only spelling storm.h already uses for others) and zeroed right
//  after, in the same order every time: nothing here shares state between
//  the four, so this is a straight-line unroll rather than a loop over a
//  table, matching the shipped body exactly.
//
//  Declared `int`-returning, not `void`, to match the existing declaration
//  in Game/gamerestartload.cpp - a C++ mangled name (this is not
//  `extern "C"`) folds in the return type, so the two would otherwise be
//  two different symbols for the same import (tools/link_check.py's own
//  "declaration disagrees with definition in another TU" class); the
//  return value itself is never read here, same as gamerestartload.cpp's
//  own four call sites.
//============================================================================

int __stdcall Storm_252(void* handle);

extern void* g_storm252HandleA;   // dword_6FAAE27C
extern void* g_storm252HandleB;   // dword_6FAAE274
extern void* g_storm252HandleC;   // dword_6FAAE278
extern void* g_storm252HandleD;   // dword_6FAAE270

void ReleaseFourStorm252HandlesB3()
{
    if (g_storm252HandleA)
    {
        Storm_252(g_storm252HandleA);
        g_storm252HandleA = 0;
    }
    if (g_storm252HandleB)
    {
        Storm_252(g_storm252HandleB);
        g_storm252HandleB = 0;
    }
    if (g_storm252HandleC)
    {
        Storm_252(g_storm252HandleC);
        g_storm252HandleC = 0;
    }
    if (g_storm252HandleD)
    {
        Storm_252(g_storm252HandleD);
        g_storm252HandleD = 0;
    }
}
