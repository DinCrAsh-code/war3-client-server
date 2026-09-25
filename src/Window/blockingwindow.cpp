//============================================================================
//  gamemain-batch-3: a modal "please wait" window - CreateWindowExA under a
//  fixed "BlockingWindow" class registered once, sized and centred to the
//  desktop, and its matching teardown.  hInstance (0x6FAD1394) is the real,
//  exported Storm/CRT global every WinMain-style entry point reads its
//  module handle from - same address family as uIDEvent/TimerFunc
//  (blockingwindowtimer.cpp).  sub_6F6BB100, the WndProc this class
//  registers, is one of the three dead-end RegisterClass procs
//  gxdeviceopenglwndproc.cpp already documents (defaults every message to
//  DefWindowProcA) - referenced here only by its raw address, the same
//  `(FnPtrType)0x6F...` idiom used for TimerFunc, since nothing in this
//  call tree ever has to execute it.
//============================================================================
#include <windows.h>

extern unsigned int g_blockingWindowRegistered;   // dword_6FAD139C
extern HWND          g_blockingWindowHwnd;         // dword_6FAD13A0
extern HINSTANCE     hInstance;                    // 0x6FAD1394

//----------------------------------------------------------------------------
//  0x6F6BB1D0 - CreateBlockingWindow.  Registers the window class exactly
//  once (skips straight to creation on every later call); on any failure
//  along the way it simply falls through to the shared exit with the
//  window handle left whatever it already was.
//----------------------------------------------------------------------------
void CreateBlockingWindow()
{
    WNDCLASSA wc;
    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC)0x6F6BB100;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = 0;
    wc.hCursor = 0;
    wc.hbrBackground = 0;
    wc.lpszMenuName = 0;
    wc.lpszClassName = "BlockingWindow";

    if (!g_blockingWindowRegistered)
    {
        if (!RegisterClassA(&wc))
            return;
    }

    RECT desktopRect;
    GetWindowRect(GetDesktopWindow(), &desktopRect);

    g_blockingWindowRegistered = 1;

    HWND hwnd = CreateWindowExA(0, "BlockingWindow", "Warcraft III",
                                0x91000000, 0, 0,
                                desktopRect.right - desktopRect.left,
                                desktopRect.bottom - desktopRect.top,
                                0, 0, hInstance, 0);
    g_blockingWindowHwnd = hwnd;
    if (hwnd == 0)
        return;

    ShowWindow(hwnd, SW_MAXIMIZE);
    UpdateWindow(g_blockingWindowHwnd);
    SetForegroundWindow(g_blockingWindowHwnd);
    SetActiveWindow(g_blockingWindowHwnd);
}

//----------------------------------------------------------------------------
//  0x6F6BB2E0 - DestroyBlockingWindow.
//----------------------------------------------------------------------------
void DestroyBlockingWindow()
{
    if (g_blockingWindowHwnd)
    {
        DestroyWindow(g_blockingWindowHwnd);
        g_blockingWindowHwnd = 0;
    }
}
