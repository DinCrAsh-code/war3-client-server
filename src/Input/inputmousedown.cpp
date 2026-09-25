//============================================================================
//  0x6F630F60 - InputMouseDownHandler, DispatchInputEvent's own case 9 -
//  the type-9 (mouse-button-down) branch this session's task named as the
//  one to follow closest.  Per docs/notes/pause-mouse-capture.md's own
//  dated section: ORs wParam's own button/modifier bits into
//  g_pumpHeldButtonMask, clamps/re-warps the cursor into the global screen
//  bound via ClampCursorToScreenBounds, and posts message type 0x0B (11)
//  through PostQueuedInputMessage - Storm's own generic per-object
//  message-subscriber-list broadcast, not anything `CFrame`/`CSimpleTop`-
//  shaped.  No store to any object's `+0x16C`
//  (`CSimpleTop::m_pCapturedFrame`'s own offset) anywhere in this function
//  or SampleModifierSnapshot/ClampCursorToScreenBounds, its own two direct
//  callees.  See docs/targets/InputEventPump.md for the honest "does not
//  resolve the investigation, and here is the next edge" writeup.
//============================================================================
#include "inputeventpump.h"

void __fastcall InputMouseDownHandler(int wParam, void* handler, int x,
                                      int y, int d)
{
    int newMask = wParam | g_pumpHeldButtonMask;
    g_pumpHeldButtonMask = newMask;

    int payload[5];
    payload[0] = g_pumpInputMode;
    payload[1] = wParam;
    payload[2] = newMask;
    payload[3] = g_pumpModifierBits;
    payload[4] = SampleModifierSnapshot();

    float fracX, fracY;
    ClampCursorToScreenBounds(x, y, &fracX, &fracY);

    int dSlot = d;
    (void)dSlot;

    PostQueuedInputMessage(handler, 0x0B, payload);
}
