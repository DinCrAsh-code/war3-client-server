//============================================================================
//  0x6F6BAD70/0x6F6BAF30 - a small, self-contained "which window"/"which
//  modifier" module in the 0x6F6BAxxx neighbourhood, a real address gap
//  away from CGxRawInputBridge's own 0x6F6Cxxxx module - own translation
//  unit per CLAUDE.md's own rule, and not merely tidiness: keeping these
//  two in the same TU as their one caller (CenterCursorInActiveWindow,
//  winmousehelpers.cpp) let this toolchain's /Ob2 inline
//  `GetSelectedWindow(0)` straight down to `g_cachedGameWindow`, losing
//  the real call the shipped, separately-compiled body still makes.
//============================================================================
#include "wininputbridge.h"

//  0x6F6BAD70 - see wininputbridge.h.
HWND __fastcall GetSelectedWindow(unsigned int which)
{
    switch (which)
    {
    case 0:  return g_cachedGameWindow;
    case 1:  return GetActiveWindow();
    case 2:  return GetForegroundWindow();
    default: return 0;
    }
}

//  0x6F6BAF30 - see wininputbridge.h.
int __fastcall IsModifierKeyDown(unsigned int which)
{
    int vk;
    switch (which)
    {
    case 0:  vk = VK_CONTROL; break;
    case 1:  vk = VK_SHIFT;   break;
    case 2:  vk = VK_MENU;    break;
    default: return 0;
    }
    return (GetKeyState(vk) & 0xF000) != 0;
}
