//============================================================================
//  0x6F6C6AA0 - CGxRawInputBridge, the real function installed into
//  dword_6FACBD74 and the actual bridge from raw Win32 messages into the
//  game's own input-event queue.  See wininputbridge.h's own header
//  comment for how this address was found, and docs/targets/
//  CGxRawInputBridge.md for the full per-message table.
//
//  __fastcall: `hWnd` in ecx, `Msg` in edx, `wParam`/`lParam` on the stack
//  (`retn 8`) - the same shape gxdeviceopenglwndproc.cpp's own generic
//  forward calls it with.
//
//  Structure: a sparse `switch (Msg)` lowers to a short `sub`/`jz` chain
//  for the handful of values below 0x1D, a further cascade of individual
//  compares for 0x10D/0x10E/0x10F/0x113/0x200 (each too far from its
//  neighbours to be worth folding into a jump table), and two real
//  jump-table switches for the dense ranges [0x51,0x105] and
//  [0x201,0x285] (docs/msvc-vc8-idioms.md, "A jump-table switch's own
//  data table shows up on one side only" and "case-body layout order is
//  not source-addressable" - do not expect this reconstruction's own case
//  body ordering to match the shipped physical layout instruction for
//  instruction; the call-target/operation content is what was checked,
//  by hand, against every one of these blocks in the dump).  Everything
//  not explicitly handled reaches the same generic forward as
//  gxdeviceopenglwndproc.cpp's own default case: if g_pDirectInputCallback
//  is set, call it with the same four arguments; otherwise
//  DefWindowProcA.
//============================================================================
#include "wininputbridge.h"

int __fastcall CGxRawInputBridge(HWND hWnd, UINT Msg, WPARAM wParam,
                                 LPARAM lParam)
{
    //  msg <= 0x1C: WM_ACTIVATEAPP and a handful of values below it.
    if (Msg <= 0x1C)
    {
        if (Msg == 0x1C)                       // WM_ACTIVATEAPP
        {
            g_appActiveFlag = (int)wParam;
            //  falls through to the generic forward below
        }
        else if (Msg == WM_SIZE || Msg == 126) // WM_SIZE / WM_DISPLAYCHANGE
        {
            //  Both share this store.  WM_SIZE (5) always updates it;
            //  WM_DISPLAYCHANGE (126, 0x7E) only when g_displayChangeGuard
            //  is set - the exact shape of the shipped `cmp .../jz` pair.
            if (Msg != 126 || g_displayChangeGuard != 0)
                g_lastDisplayChangeParam = (int)lParam;
            //  falls through to the generic forward below
        }
        else if (Msg == WM_ACTIVATE)           // 6
        {
            int activating = (LOWORD(wParam) != 0);
            g_heldMouseButtons = 0;
            PostInputEvent(6, activating, 0, 0, 0);
            //  falls through to the generic forward below
        }
        else if (Msg == WM_CLOSE)              // 16 - swallowed entirely,
        {                                      // no forward at all: the
            PostInputEvent(5, 0, 0, 0, 0);      // game decides when to
            return 0;                            // really close
        }
        //  every other value in [0,0x1C) reaches the generic forward
        //  directly, same as an unhandled default case
        goto GenericForward;
    }

    //  0x1D..0x10C: the first dense jump-table range.
    if (Msg < 0x10D)
    {
        if (Msg == 0x51)                       // WM_INPUTLANGCHANGE
        {
            PostInputEvent(3, (int)Msg, (int)wParam, (int)lParam, 0);
            return 0;
        }

        if (Msg == WM_CHAR)                    // 0x102
        {
            if (wParam < ' ')
                goto GenericForward;

            unsigned int lowByte = wParam & 0xFF;
            unsigned int translated = (unsigned int)wParam;

            if (lowByte >= 0x80)
            {
                unsigned int codepage = GetACP();
                if (codepage == 1250)
                    translated = g_charMapCP1250[lowByte];
                else if (codepage == 874)          // Thai
                    translated = g_charMapCPThai[lowByte];
                else if (codepage == 1251)          // Cyrillic
                    translated = g_charMapCP1251[lowByte];
                else if (codepage == 1252)            // Western/Latin-1
                {
                    if (lowByte < 0xA0)
                        translated = g_charMapCP1252[wParam & 0x7F];
                    //  else: passes through unchanged
                }
                else if (codepage == 1253)              // Greek
                    translated = g_charMapCP1253[lowByte];
                //  any other codepage: passes through unchanged
            }

            PostInputEvent(1, (int)translated, (short)LOWORD(lParam), 0, 0);
            return 0;
        }

        if (Msg == WM_KEYDOWN || Msg == WM_KEYUP ||
            Msg == WM_SYSKEYDOWN || Msg == WM_SYSKEYUP)
        {
            unsigned int mapped;
            if (!MapVKeyToInternal((int)wParam, &mapped))
                goto GenericForward;

            int isDown = (Msg == WM_KEYDOWN || Msg == WM_SYSKEYDOWN);
            PostInputEvent(isDown ? 7 : 8, (int)mapped,
                          (short)LOWORD(lParam), 0, 0);

            //  Two further special cases layered on top of the same
            //  posted event, both still reached from this one message.
            //  0x303 is not resolved further by this session - see
            //  docs/targets/CGxRawInputBridge.md.
            if (mapped == 0x303)
            {
                if (IsModifierKeyDown(2))          // Alt held
                    goto GenericForward;
                return 0;
            }
            if (mapped == 2 && g_pDirectInputCallback != 0)   // Alt itself
                return g_pDirectInputCallback(hWnd, Msg, wParam, lParam);
            return 0;
        }

        goto GenericForward;
    }

    if (Msg == WM_IME_STARTCOMPOSITION)        // 0x10D
    {
        if (g_imeWindowActive != 0)
            PostInputEvent(3, (int)Msg, (int)wParam, (int)lParam, 0);
        return 0;
    }

    if (Msg == WM_IME_ENDCOMPOSITION)          // 0x10E
    {
        if (g_imeWindowActive != 0)
            PostInputEvent(3, (int)Msg, (int)wParam, (int)lParam, 0);
        return 0;
    }

    if (Msg == WM_IME_COMPOSITION)             // 0x10F
    {
        if (g_imeWindowActive != 0)
            PostInputEvent(3, (int)Msg, (int)wParam, (int)lParam, 0);
        return 0;
    }

    if (Msg == WM_TIMER)                       // 0x113
    {
        if (wParam == 0xF060)
        {
            //  the same swallowed-close post WM_CLOSE itself uses
            PostInputEvent(5, 0, 0, 0, 0);
            return 0;
        }
        if (g_pDirectInputCallback != 0)
            return g_pDirectInputCallback(hWnd, Msg, wParam, lParam);
        if (wParam != 0xF100)
            return (int)DefWindowProcA(hWnd, Msg, wParam, lParam);
        return 0;
    }

    if (Msg == WM_MOUSEMOVE)                   // 0x200
    {
        POINT pt;
        GetCursorPos(&pt);

        if (g_relativeMouseLookMode != 0)
        {
            if (pt.x != g_lastCenterX || pt.y != g_lastCenterY)
            {
                PostInputEvent(0xC, 0, pt.x - g_lastCenterX,
                              pt.y - g_lastCenterY, 0);
                CenterCursorInActiveWindow();
            }
        }
        else
        {
            ScreenToClient(hWnd, &pt);
            PostInputEvent(0xA, 0, pt.x, pt.y, 0);
            CacheLastMousePosition(hWnd, &pt);
        }
        goto GenericForward;
    }

    //  0x201..0x285: the second dense jump-table range.
    if (Msg >= 0x201 && Msg <= 0x285)
    {
        switch (Msg)
        {
        case WM_LBUTTONDOWN: case WM_LBUTTONUP:
        case WM_RBUTTONDOWN: case WM_RBUTTONUP:
        case WM_MBUTTONDOWN: case WM_MBUTTONUP:
        case WM_XBUTTONDOWN: case WM_XBUTTONUP:
        {
            unsigned int bit;
            if (!MapButtonMessageToBit(Msg, &bit, (unsigned int)wParam))
                goto GenericForward;

            int isDown = (Msg == WM_LBUTTONDOWN || Msg == WM_MBUTTONDOWN ||
                         Msg == WM_RBUTTONDOWN || Msg == WM_XBUTTONDOWN);

            if (isDown)
            {
                if (g_heldMouseButtons == 0)
                    SetCapture(hWnd);
                g_heldMouseButtons |= (unsigned int)wParam;
            }
            else
            {
                g_heldMouseButtons &= ~(unsigned int)wParam;
                if (g_heldMouseButtons == 0)
                {
                    g_cursorWarpReentryGuard = 1;
                    ReleaseCapture();
                    g_cursorWarpReentryGuard = 0;
                }
            }

            short x = (short)LOWORD(lParam);
            short y = (short)HIWORD(lParam);
            PostInputEvent(isDown ? 9 : 13, (int)wParam, x, y, 0);
            return 0;
        }

        case WM_MOUSEWHEEL:                    // 0x20A
        {
            short delta = (short)HIWORD(wParam);
            short x = (short)LOWORD(lParam);
            short y = (short)HIWORD(lParam);
            PostInputEvent(0xB, delta, x, y, 0);
            return 0;
        }

        case WM_CAPTURECHANGED:                // 0x215
            if (g_cursorWarpReentryGuard != 0)
                goto GenericForward;
            {
                g_heldMouseButtons = 0;
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hWnd, &pt);
                PostInputEvent(0, 0, pt.x, pt.y, 0);
            }
            return 0;

        case WM_IME_SETCONTEXT:                // 0x281
            if (g_imeWindowActive != 0)
                PostInputEvent(3, (int)Msg, (int)wParam, (int)lParam, 0);
            goto GenericForward;

        case WM_IME_NOTIFY:                    // 0x282
            if (g_imeWindowActive != 0)
            {
                PostInputEvent(3, (int)Msg, (int)wParam, (int)lParam, 0);
                return 0;
            }
            goto GenericForward;

        case 0x283: case 0x284: case 0x285:    // three further IME
            return 0;                            // messages, swallowed with
                                                // no event and no forward

        default:
            goto GenericForward;
        }
    }

GenericForward:
    if (g_pDirectInputCallback != 0)
        return g_pDirectInputCallback(hWnd, Msg, wParam, lParam);
    return (int)DefWindowProcA(hWnd, Msg, wParam, lParam);
}
