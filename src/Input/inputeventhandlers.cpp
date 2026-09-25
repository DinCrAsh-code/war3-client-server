//============================================================================
//  DispatchInputEvent's own 13 non-mouse-down case handlers (types 0-4,
//  6-8, 10-13), plus the three small shared helpers they and
//  InputMouseDownHandler all call.  Every one of these posts through
//  PostQueuedInputMessage with its own message-type constant and payload
//  shape - see inputeventpump.h for what each type constant is believed to
//  mean and docs/targets/InputEventPump.md for the full per-case table.
//
//  Several bodies keep a store that is never read again afterward (a copy
//  of an argument, a float conversion, a snapshot result) - the shipped
//  compiler keeps every one of these too, the same "cannot prove a value
//  with no further use is dead once its computation could raise an FP
//  exception or a call has already run" class CLAUDE.md's own
//  Definition-of-Done section and this repo's existing float-comparison
//  idioms document.  Marked `(void)` rather than removed, so the
//  instruction stream still has something to match.
//============================================================================
#include "inputeventpump.h"
#include <windows.h>

//  0x6F630D90 - see inputeventpump.h.
int SampleModifierSnapshot()
{
    if (g_pumpInputMode != 1)
        return 0;
    return 2;
}

//  0x6F631200 - see inputeventpump.h.
void ResetPumpAccumulators()
{
    g_pumpHeldButtonMask = 0;
    g_pumpModifierBits = 0;
}

//  0x6F630DB0 - see inputeventpump.h.
int QuitRequestGate()
{
    PumpQuitCallbackFn cb = g_pumpQuitCallback;
    if (cb)
        return cb(g_pumpQuitCallbackContext);
    return 1;
}

//  0x6F630B80 - see inputeventpump.h.
#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }               \
    }

ADDR_THUNK(void PumpModifierChangeGate(), 0x6F630B80)

#undef ADDR_THUNK

//  0x6F631420 - type 0.  Drains g_pumpHeldButtonMask one bit at a time
//  (lowest set bit first, the standard `x & -x` isolation, spelled here as
//  the shipped `((x-1)^x)&x`), re-invoking InputEventType13Handler for
//  each until nothing is left held.
void __stdcall InputEventType0Handler(void* handler, int b, int c)
{
    while (g_pumpHeldButtonMask != 0)
    {
        unsigned int mask = g_pumpHeldButtonMask;
        unsigned int bit = ((mask - 1) ^ mask) & mask;
        unsigned int tick = GetTickCount();
        InputEventType13Handler(handler, (int)bit, b, c, (int)tick);
    }
}

//  0x6F630DD0 - type 1.
void __stdcall InputEventType1Handler(void* handler, int a, int b)
{
    int payload[3];
    payload[0] = a;
    payload[1] = g_pumpModifierBits;
    payload[2] = b;
    PostQueuedInputMessage(handler, 1, payload);
}

//  0x6F630E00 - type 2.  `a` is really a `const unsigned short*` (a small
//  packed array the event's own producer built) and `count` its length -
//  one message posted per element.
void __stdcall InputEventType2Handler(void* handler, int a, int count)
{
    const unsigned short* values = (const unsigned short*)a;
    for (int i = 0; i < count; i++)
    {
        int payload[3];
        payload[0] = values[i];
        payload[1] = g_pumpModifierBits;
        payload[2] = 1;
        PostQueuedInputMessage(handler, 1, payload);
    }
}

//  0x6F630E50 - type 3 (0x1B).
void __stdcall InputEventType3Handler(void* handler, int a, int b, int c)
{
    int payload[4];
    payload[0] = a;
    payload[1] = b;
    payload[2] = c;
    payload[3] = (int)GetACP();
    PostQueuedInputMessage(handler, 0x1B, payload);
}

//  0x6F630E80 - type 4 (0x1C).
void __stdcall InputEventType4Handler(void* handler, int a, int b)
{
    int payload[2];
    payload[0] = a;
    payload[1] = b;
    PostQueuedInputMessage(handler, 0x1C, payload);
}

//  0x6F631460 - type 6.
void __stdcall InputEventType6Handler(void* handler, int a)
{
    ResetPumpAccumulators();
    int payload[1];
    payload[0] = a;
    PostQueuedInputMessage(handler, 2, payload);
    PumpModifierChangeGate();
}

//  0x6F630EC0 - type 7 (message type 8).  `a` in [0,2] sets the matching
//  bit in g_pumpModifierBits before the post.
void __stdcall InputEventType7Handler(void* handler, int a, int b, int d)
{
    if ((unsigned int)a <= 2)
        g_pumpModifierBits |= (1 << a);

    int payload[4];
    payload[0] = a;
    payload[1] = g_pumpModifierBits;
    payload[2] = b;
    payload[3] = d;
    PostQueuedInputMessage(handler, 8, payload);
}

//  0x6F630F10 - type 8 (message type 9), the mirror of type 7: clears the
//  bit instead of setting it.
void __stdcall InputEventType8Handler(void* handler, int a, int b, int d)
{
    if ((unsigned int)a <= 2)
        g_pumpModifierBits &= ~(1 << a);

    int payload[4];
    payload[0] = a;
    payload[1] = g_pumpModifierBits;
    payload[2] = b;
    payload[3] = d;
    PostQueuedInputMessage(handler, 9, payload);
}

//  0x6F630FD0 - type 10 (message type 0xC).  `b`/`c` are the event's own
//  x/y - fed to ClampCursorToScreenBounds for its cursor-warp side effect;
//  its own two fractional outputs are never read afterward here, the same
//  as every other caller in this module (InputMouseDownHandler included).
void __stdcall InputEventType10Handler(void* handler, int b, int c)
{
    int payload[5];
    payload[0] = g_pumpInputMode;
    payload[1] = 0;
    payload[2] = g_pumpHeldButtonMask;
    payload[3] = g_pumpModifierBits;
    payload[4] = SampleModifierSnapshot();

    float fracX, fracY;
    ClampCursorToScreenBounds(b, c, &fracX, &fracY);

    PostQueuedInputMessage(handler, 0xC, payload);
}

//  0x6F631190 - type 11 (message type 0x10).
void __stdcall InputEventType11Handler(void* handler, int a, int b, int c)
{
    int payload[5];
    payload[0] = g_pumpInputMode;
    payload[1] = 0;
    payload[2] = g_pumpHeldButtonMask;
    payload[3] = g_pumpModifierBits;

    int aCopy = a;
    (void)aCopy;

    payload[4] = SampleModifierSnapshot();

    float fracX, fracY;
    ClampCursorToScreenBounds(b, c, &fracX, &fracY);

    PostQueuedInputMessage(handler, 0x10, payload);
}

//  0x6F631040 - type 12 (message type 0xD).  Converts b/c to floats
//  directly rather than going through ClampCursorToScreenBounds - and,
//  same as that function's own two outputs everywhere else, never reads
//  either result again.
void __stdcall InputEventType12Handler(void* handler, int b, int c)
{
    int payload[5];
    payload[0] = g_pumpInputMode;
    payload[1] = 0;
    payload[2] = g_pumpHeldButtonMask;
    payload[3] = g_pumpModifierBits;
    payload[4] = SampleModifierSnapshot();

    float fb = (float)b;
    float fc = (float)c;
    (void)fb;
    (void)fc;

    PostQueuedInputMessage(handler, 0xD, payload);
}

//  0x6F6310A0 - type 13 (message type 0xE), InputEventType0Handler's own
//  recursive target and DispatchInputEvent's own direct case.  AND's the
//  matching bits out of g_pumpHeldButtonMask.
void __stdcall InputEventType13Handler(void* handler, int a, int b, int c,
                                       int d)
{
    g_pumpHeldButtonMask &= ~a;

    int payload[5];
    payload[0] = g_pumpInputMode;
    payload[1] = a;
    payload[2] = g_pumpHeldButtonMask;
    payload[3] = g_pumpModifierBits;
    payload[4] = SampleModifierSnapshot();

    float fracX, fracY;
    ClampCursorToScreenBounds(b, c, &fracX, &fracY);

    int dCopy = d;
    (void)dCopy;

    PostQueuedInputMessage(handler, 0xE, payload);
    PumpModifierChangeGate();
}
