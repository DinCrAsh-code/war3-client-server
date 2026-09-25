//============================================================================
//  0x6F631600/0x6F631490 - the real drain loop and dispatcher on top of
//  PumpAndPopInputEvent (Window/wininputbridge.h).  This is the headline
//  deliverable of this session's task - see inputeventpump.h's own header
//  comment and docs/targets/InputEventPump.md for the full trace.
//============================================================================
#include "inputeventpump.h"
#include "../Window/wininputbridge.h"
#include "../Frame/frame.h"     // g_pSimpleTop, CSimpleTop::m_pCapturedFrame

//  ==========================================================================
//  DELIBERATE FEATURE (2026-08-28) - second attempt at "let the camera move
//  while the game is paused."  See gameuisetgamepaused.cpp's own
//  CAMERA_FEATURE_RELEASE_CAPTURE_ON_PAUSE for the first attempt and why it
//  was not enough: a one-shot release the moment pause begins cannot survive
//  capture getting stuck again on a *later* click during the pause, and the
//  operator confirmed live that it did not fix anything - clicks still did
//  nothing with that flag alone armed.
//
//  This is every mouse-down's own entry point instead of pause's own entry
//  point - `DispatchInputEvent`'s type-9 case, which runs on literally every
//  physical mouse-down, in and out of pause, forever (PumpAndDispatchInputEvents
//  drains this every frame - see this file's own header comment).  While
//  paused, release whatever is currently captured *before* processing the new
//  press, using the same already-EXACT `CSimpleTop::RemoveFrameFromLayer`
//  release shape as the first attempt.  This defeats a capture that gets
//  re-stuck on every subsequent click, not just the one held when pause
//  began, at the cost of also releasing a still-legitimate capture that
//  happens to be held by the Esc-menu's own UI at the exact instant of a new
//  press elsewhere (untested trade-off - if it breaks the pause menu's own
//  click-drag feedback, that is the next thing to isolate).
//
//  Off by default, same as the first attempt.  Never tested live.
#define CAMERA_FEATURE_RELEASE_CAPTURE_PER_CLICK_WHILE_PAUSED   0
//  ==========================================================================

//  Storm.dll ordinal 465, same as every other file in this repo that calls
//  it.
void __stdcall SErrSetLastError(unsigned int code);

//  0x6F631490 - see inputeventpump.h.
void __fastcall DispatchInputEvent(void* handler, int type, int* payload,
                                   int* outQuitRequested)
{
    if (handler == 0)
    {
        SErrSetLastError(0x57);
        return;
    }

    if ((unsigned int)type > 0x0E)
        return;

    switch (type)
    {
    case 0:
        InputEventType0Handler(handler, payload[1], payload[2]);
        break;

    case 1:
        InputEventType1Handler(handler, payload[0], payload[1]);
        break;

    case 2:
        InputEventType2Handler(handler, payload[0], payload[1]);
        break;

    case 3:
        InputEventType3Handler(handler, payload[0], payload[1], payload[2]);
        break;

    case 4:
        InputEventType4Handler(handler, payload[0], payload[1]);
        break;

    case 5:
        if (QuitRequestGate())
        {
            QuitBroadcastRelay();
            *outQuitRequested = 1;
        }
        break;

    case 6:
        InputEventType6Handler(handler, payload[0]);
        break;

    case 7:
        InputEventType7Handler(handler, payload[0], payload[1], payload[3]);
        break;

    case 8:
        InputEventType8Handler(handler, payload[0], payload[1], payload[3]);
        break;

    case 9:
#if CAMERA_FEATURE_RELEASE_CAPTURE_PER_CLICK_WHILE_PAUSED
        {
            CGameUI* ui = (CGameUI*)GetGameUI(0, 0);
            if (ui != 0 && ui->m_pausedByPlayer != 0 &&
                g_pSimpleTop != 0 && g_pSimpleTop->m_pCapturedFrame != 0)
            {
                g_pSimpleTop->m_pCapturedFrame->NotifyCaptureLost();
                g_pSimpleTop->m_pCapturedFrame = 0;
            }
        }
#endif
        InputMouseDownHandler(payload[0], handler, payload[1], payload[2],
                              payload[3]);
        break;

    case 10:
        InputEventType10Handler(handler, payload[1], payload[2]);
        break;

    case 12:
        InputEventType12Handler(handler, payload[1], payload[2]);
        break;

    case 13:
        InputEventType13Handler(handler, payload[0], payload[1], payload[2],
                                payload[3]);
        break;

    case 11:
        InputEventType11Handler(handler, payload[0], payload[1], payload[2]);
        break;

    case 14:
        if (QuitRequestGate())
        {
            QuitBroadcastRelay();
            *outQuitRequested = 1;
        }
        break;
    }
}

//  0x6F631600 - see inputeventpump.h.  The return value is "did this call
//  process at least one event" (0 if the ring, and the whole Win32 message
//  queue behind it, were completely empty), not related to
//  *outQuitRequested at all - that is DispatchInputEvent's own out-param,
//  threaded straight through.
int __fastcall PumpAndDispatchInputEvents(void* handler, int* outQuitRequested)
{
    if (handler == 0)
    {
        SErrSetLastError(0x57);
        return 0;
    }

    int type;
    int payload[4];
    int processedAny = 0;

    if (!PumpAndPopInputEvent(&type, &payload[0], &payload[1], &payload[2],
                              &payload[3]))
        return processedAny;

    processedAny = 1;
    do
    {
        DispatchInputEvent(handler, type, payload, outQuitRequested);
    }
    while (PumpAndPopInputEvent(&type, &payload[0], &payload[1], &payload[2],
                                &payload[3]));

    return processedAny;
}
