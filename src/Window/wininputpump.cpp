//============================================================================
//  The real consumer of the input-event ring PostInputEvent
//  (winmousehelpers.cpp) writes into - docs/notes/pause-mouse-capture.md's
//  "Claimed... the ring-buffer consumer" section, and the headline target
//  of this session.  `PopInputEvent` is the second (and last) place besides
//  PostInputEvent's own dedup that ever advances g_inputEventReadIndex, and
//  `PumpAndPopInputEvent` is what actually calls it once per "give me the
//  next event" request - see docs/targets/InputEventPump.md for the full
//  trace to (and past) the type-9/mouse-down dispatch this session followed
//  as far as it goes.
//
//  `PretranslateWindowMessage` (0x6F6C2810) is a naked redirect.  It is the
//  Win32 "accelerator table + modeless-dialog navigation" pretranslate every
//  drained message runs through ahead of TranslateMessage/DispatchMessage -
//  real, substantial keyboard-navigation plumbing (TranslateAcceleratorA
//  over every registered accelerator table, IsDialogMessageA over every
//  registered modeless dialog) with nothing to do with mouse capture, and a
//  genuinely separate chunk of work from this session's own task -
//  CLAUDE.md's own "thunk a deep, boring, generic branch" allowance.
//============================================================================
#include "wininputbridge.h"

//  Reconstructed for real in src/Sync/critsec.cpp; declared locally the same
//  way threadwaitpool.cpp's own header comment explains (an ordinary
//  __fastcall C++ definition there, not extern "C" - a shared critsec.h
//  does not exist yet).
void __fastcall ThreadSleep(unsigned int);

//  Storm.dll ordinal 465, same as every other file in this repo that calls
//  it (Misc/lockedkeylist.cpp's own comment has the address).
void __stdcall SErrSetLastError(unsigned int code);

//  0x6F6CDEB0 - see wininputbridge.h.
int ShouldSleepForInactivity()
{
    return g_shouldSleepForInactivity;
}

//  0x6F6CDED0 - see wininputbridge.h.
int GetInactivitySleepMs()
{
    return g_inactivitySleepMs;
}

//  0x6F6C60B0 - see wininputbridge.h.
int __stdcall PopInputEvent(int* outType, int* outA, int* outB, int* outC,
                            int* outD)
{
    unsigned int idx = g_inputEventReadIndex;
    if (idx == g_inputEventWriteIndex)
        return 0;

    SInputEvent* slot = &g_inputEventRing[idx];
    *outType = slot->m_type;
    *outA = slot->m_a;
    *outB = slot->m_b;
    *outC = slot->m_c;
    *outD = slot->m_d;

    g_inputEventReadIndex = (idx == 0xF) ? 0 : idx + 1;
    return 1;
}

//  0x6F6C6130 - see wininputbridge.h.  `field` selects which payload word
//  of every unread event gets overwritten (0=a .. 3=d); the loop walks the
//  read index forward to (not including) the write index without consuming
//  anything - a broadcast overwrite, not a pop.
void __fastcall StampPendingInputEventField(int value, int field)
{
    unsigned int idx = g_inputEventReadIndex;
    unsigned int end = g_inputEventWriteIndex;
    if (idx == end)
        return;

    int* base = &g_inputEventRing[0].m_a;
    do
    {
        base[idx * 5 + field] = value;
        idx = (idx == 0xF) ? 0 : idx + 1;
    } while (idx != end);
}

//  0x6F6C6720 - see wininputbridge.h.
int __fastcall PumpAndPopInputEvent(int* outType, int* outA, int* outB,
                                    int* outC, int* outD)
{
    int pending = g_lastDisplayChangeParam;
    *outType = -1;

    if (pending != 0)
    {
        short height = (short)(pending >> 16);
        short width = (short)pending;

        //  Round-tripped through the FPU and never read again - the shipped
        //  compiler keeps both truncations anyway (a float->int conversion
        //  can raise an FP exception, so /O2 cannot prove either is dead).
        int roundedHeight = (int)(float)height;
        int roundedWidth = (int)(float)width;
        (void)roundedHeight;
        (void)roundedWidth;

        *outA = width;
        *outB = height;
        *outC = 0;
        *outD = 0;
        *outType = 4;
        g_lastDisplayChangeParam = 0;
        return 1;
    }

    //  Already had something before any pumping at all - the one path that
    //  skips StampPendingInputEventField entirely, straight to the pop.
    if (g_inputEventReadIndex != g_inputEventWriteIndex)
        return PopInputEvent(outType, outA, outB, outC, outD);

    {
        MSG msg;
        int havePending = PeekMessageA(&msg, 0, 0, 0, PM_NOREMOVE);

        if (g_inputEventReadIndex == g_inputEventWriteIndex)
        {
            if (!havePending)
            {
                if (g_appActiveFlag == 0 && ShouldSleepForInactivity())
                    ThreadSleep(GetInactivitySleepMs());
                return 0;
            }

            for (;;)
            {
                if (!GetMessageA(&msg, 0, 0, 0))
                {
                    *outType = 0xE;
                    return 1;
                }

                if (!PretranslateWindowMessage(&msg) &&
                    g_inputEventReadIndex == g_inputEventWriteIndex)
                {
                    TranslateMessage(&msg);
                    DispatchMessageA(&msg);
                }

                if (g_inputEventReadIndex != g_inputEventWriteIndex)
                    break;

                PeekMessageA(&msg, 0, 0, 0, PM_NOREMOVE);

                if (g_inputEventReadIndex != g_inputEventWriteIndex)
                    break;
            }
        }

        //  The ring became non-empty as a side effect of pumping - stamp the
        //  message that (indirectly) produced it before popping.
        StampPendingInputEventField(msg.time, 3);
        return PopInputEvent(outType, outA, outB, outC, outD);
    }
}

//  0x6F6C2810 - see wininputbridge.h and this file's own header comment.
//  `__fastcall(ecx=msg)`, bare `retn` (0 stack bytes) per its own dump.
#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }               \
    }

ADDR_THUNK(int __fastcall PretranslateWindowMessage(const MSG*), 0x6F6C2810)

#undef ADDR_THUNK

//  0x6F6C69A0 - see wininputbridge.h.
void __fastcall WarpCursorTo(int x, int y)
{
    HWND hWnd = GetSelectedWindow(0);

    POINT point;
    point.x = x;
    point.y = y;
    CacheLastMousePosition(hWnd, &point);

    ClientToScreen(hWnd, &point);
    SetCursorPos(point.x, point.y);
}

//  0x6F6C69F0 - see wininputbridge.h.  `Rect` (g_cachedClientRect) is a
//  module global, not a local - it is what makes this a *cache*: once
//  right/bottom are both nonzero, GetClientRect is never called again
//  anywhere in this call tree.
int __fastcall GetCachedClientRect(int* out)
{
    if (out == 0)
    {
        SErrSetLastError(0x57);
        return 0;
    }

    if (g_cachedClientRect[2] == 0 || g_cachedClientRect[3] == 0)
    {
        RECT rect;
        if (!GetClientRect(GetSelectedWindow(0), &rect))
            return 0;

        g_cachedClientRect[0] = rect.left;
        g_cachedClientRect[1] = rect.top;
        g_cachedClientRect[2] = rect.right;
        g_cachedClientRect[3] = rect.bottom;
    }

    out[0] = g_cachedClientRect[0];
    out[1] = g_cachedClientRect[1];
    out[2] = g_cachedClientRect[2];
    out[3] = g_cachedClientRect[3];
    return 1;
}
