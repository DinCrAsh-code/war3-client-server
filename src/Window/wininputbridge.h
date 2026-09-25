//============================================================================
//  CGxRawInputBridge (0x6F6C6AA0) - the function really installed into
//  dword_6FACBD74 (gxdeviceopengl.h), and the actual bridge from raw Win32
//  messages into the game's own input-event queue.  This is Piece 2 of
//  this session's task - see docs/notes/pause-mouse-capture.md's dated
//  section for the full trace from CGxDeviceOpenGl's vtable slot +0xC down
//  to this address (sub_6F52AD00 -> sub_6F526D10 -> sub_6F009580, a
//  literal, unconditional `mov edx, offset sub_6F6C6AA0` ahead of the
//  install call - the same handler gets installed whichever GX backend the
//  game creates).
//
//  Every Win32 mouse and keyboard message WndProc does not special-case
//  reaches here (gxdeviceopenglwndproc.cpp's own generic forward).  This
//  function itself never touches `CSimpleTop::m_pCapturedFrame` (+0x16C on
//  any object) or anything Frame/CSimpleTop-shaped at all - it works at
//  the raw Win32 level (SetCapture/ReleaseCapture, a hand-rolled 15-slot
//  input-event ring buffer) and hands normalised events off through
//  PostInputEvent for some *other*, still-unreconstructed consumer to
//  drain.  See docs/targets/CGxRawInputBridge.md for the full per-message
//  table and the honest "does not resolve the investigation" conclusion.
//============================================================================
#ifndef WININPUTBRIDGE_H
#define WININPUTBRIDGE_H

//  WM_XBUTTONDOWN/UP and friends are gated behind _WIN32_WINNT >= 0x0500 in
//  vs8's own PlatformSDK headers; this module reads several of them
//  (real messages the shipped binary itself switches on, not a target-
//  side artifact), so raise the gate ahead of the include rather than
//  hand-declare each constant.
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#include <windows.h>

//----------------------------------------------------------------------------
//  dword_6FAD1490 - a 15-slot ring buffer of normalised input events,
//  dword_6FAD148C its write index and dword_6FAD1488 its read/consumed
//  index (PostInputEvent's own dedup: when the write index catches back up
//  to the read index at the top of the ring, the read index is pushed
//  forward by one instead of wrapping over unread data).  Every event is
//  five dwords - a type/opcode and four opaque payload words, meaning
//  varies per opcode and is not otherwise recoverable from this call tree.
//----------------------------------------------------------------------------
struct SInputEvent
{
    int m_type;
    int m_a, m_b, m_c, m_d;
};

extern SInputEvent  g_inputEventRing[16];      // dword_6FAD1490 - 16 slots,
                                               // not 15: the write index
                                               // wraps at 0xF (15), so index
                                               // 15 itself is a real slot
extern unsigned int g_inputEventWriteIndex;    // dword_6FAD148C
extern unsigned int g_inputEventReadIndex;     // dword_6FAD1488

//  0x6F6C6040 - append one event to the ring, `__stdcall` (`retn 14h`,
//  all five words on the stack - no register argument).
void __stdcall PostInputEvent(int type, int a, int b, int c, int d);

//----------------------------------------------------------------------------
//  The rest of this cluster's own module-global state (0x6F6BAxxx/
//  0x6F6Cxxxx neighbourhood - one raw-input module, reached only from
//  CGxRawInputBridge and its own direct callees).
//----------------------------------------------------------------------------
extern int g_lastCenterX, g_lastCenterY;           // dword_6FAD1474/1478 -
                                                    // CenterCursorInActiveWindow's
                                                    // own last-centred point
extern HWND g_lastMouseWindow;                     // dword_6FAD147C
extern int  g_lastMouseX, g_lastMouseY;            // dword_6FAD1480/1484 -
                                                    // last known client-space
                                                    // cursor position, per
                                                    // window
extern unsigned int g_heldMouseButtons;            // dword_6FAD15D4 - live
                                                    // bitmask of buttons
                                                    // currently down (also
                                                    // the SetCapture/
                                                    // ReleaseCapture guard)
extern int  g_cursorWarpReentryGuard;              // dword_6FAD15D0 -
                                                    // WM_CAPTURECHANGED's own
                                                    // "don't re-centre while
                                                    // already centring" guard
extern int  g_imeWindowActive;                     // dword_6FAD15F0 - guards
                                                    // the two IME-message
                                                    // (WM_IME_SETCONTEXT/
                                                    // WM_IME_NOTIFY) posts
extern int (__stdcall *g_pDirectInputCallback)(    // dword_6FAD15F4 - a
    HWND, UINT, WPARAM, LPARAM);                   // second, unqueued
                                                    // callback some key
                                                    // events bypass the ring
                                                    // buffer for entirely -
                                                    // and whose return
                                                    // value CGxRawInputBridge
                                                    // itself propagates
                                                    // onward as its own
extern int  g_lastDisplayChangeParam;              // dword_6FAD15E8 -
                                                    // WM_SIZE/WM_DISPLAYCHANGE's
                                                    // shared lParam store
extern int  g_displayChangeGuard;                  // dword_6FA9E7A0
extern int  g_appActiveFlag;                       // dword_6FA9E7A4 -
                                                    // WM_ACTIVATEAPP's wParam
extern int  g_relativeMouseLookMode;               // dword_6FAD15EC -
                                                    // WM_MOUSEMOVE's own
                                                    // mode switch: nonzero
                                                    // means "compute a
                                                    // delta from the last
                                                    // centred point and
                                                    // re-centre" (FPS-camera
                                                    // relative look), zero
                                                    // means ordinary
                                                    // absolute
                                                    // ScreenToClient
                                                    // positioning
extern HWND g_cachedGameWindow;                    // dword_6FAD1398 - never
                                                    // written anywhere in
                                                    // this module's own call
                                                    // tree; out of scope

//  0x6F6BAD70 - "which window": 0 = a cached handle (dword_6FAD1398),
//  1 = GetActiveWindow(), 2 = GetForegroundWindow(), anything else = NULL.
//  `ecx` selects, `retn`.
HWND __fastcall GetSelectedWindow(unsigned int which);

//  0x6F6BAF30 - a modifier-key probe over the same 0/1/2 selector shape:
//  0 = VK_MENU (Alt), 1 = VK_SHIFT, 2 = VK_CONTROL.  Returns 1 if the high
//  bit of GetKeyState() is set (key currently down), canonicalised to 0/1
//  through the usual `and 0xF000`/`neg`/`sbb`/`neg` idiom.  `ecx` selects,
//  `retn`.
int __fastcall IsModifierKeyDown(unsigned int which);

//  0x6F6C66E0 - re-centre the cursor over whichever window
//  GetSelectedWindow(0) names, remembering the centre point in
//  g_lastCenterX/Y.
void CenterCursorInActiveWindow();

//  0x6F6C6680 - cache (hWnd, point.x, point.y) as the last known mouse
//  position.  The real body reads `ecx` = hWnd (ordinary __fastcall) but
//  `eax` = the `POINT*` - no MSVC convention places a second argument
//  there, so this declaration (`edx`, the convention's real second slot)
//  cannot reproduce that one instruction-level detail; see
//  wininputbridgeleaves.cpp's own comment on the definition and its
//  BEHAVIOUR entry in funcmap.py.
void __fastcall CacheLastMousePosition(HWND hWnd, const POINT* point);

//  0x6F6C65A0 - map a raw button-down/up message id to this module's own
//  1/2/4/8/0x10 bit (left/middle/right/X1/X2), writing it through an
//  out-param and returning 1, or writing 0 and returning 0 for anything
//  unmapped.  The real body's own convention is genuinely non-standard
//  (the message id arrives in `eax`, which no MSVC calling convention
//  places a first argument in - out-param in `ecx`, one stack dword for
//  the WM_XBUTTON* case's packed hi-word button id, `retn 4`), so this
//  declaration is an ordinary `__fastcall` shim: two extra `mov`s
//  (ecx->eax, edx->ecx) ahead of the transcribed body reconcile the two
//  conventions.  IDENTICAL, not EXACT, by construction - see
//  wininputbridgeleaves.cpp's own comment on the definition.
int __fastcall MapButtonMessageToBit(unsigned int msg, unsigned int* outBit,
                                     unsigned int xButtonInfo);

//  0x6F6C6160 - see winvkeyremap.cpp's own header comment.  Same
//  non-standard "second argument through eax" shape as
//  CacheLastMousePosition; declared with an ordinary `edx` second
//  parameter instead.
int __fastcall MapVKeyToInternal(int vkey, unsigned int* out);

//  The five codepage-specific high-byte character remap tables WM_CHAR's
//  own case reads (0x6FA9E828/6FA9F428/6FA9EC28/6FA9E7A8/6FA9F028).  Real
//  addresses, real DATA rows (funcmap.py) - only the *contents* are out of
//  this session's scope (each is a real per-codepage lookup table this
//  session did not pull the bytes of).  128 entries covers every index
//  this call tree ever reads (`lowByte`/`wParam & 0x7F`, both <128).
extern const unsigned int g_charMapCP1250[128];    // dword_6FA9E828
extern const unsigned int g_charMapCPThai[128];    // dword_6FA9F428
extern const unsigned int g_charMapCP1251[128];    // dword_6FA9EC28
extern const unsigned int g_charMapCP1252[128];    // dword_6FA9E7A8
extern const unsigned int g_charMapCP1253[128];    // dword_6FA9F028

//  0x6F6C6AA0 - see this file's own header comment.  `__fastcall`:
//  `hWnd` in ecx, `Msg` in edx, `wParam`/`lParam` on the stack (`retn 8`).
int __fastcall CGxRawInputBridge(HWND hWnd, UINT Msg, WPARAM wParam,
                                 LPARAM lParam);

//============================================================================
//  The ring's real consumer side - docs/notes/pause-mouse-capture.md's own
//  "Claimed... the ring-buffer consumer" section.  `PostInputEvent` above is
//  the only writer; these are the only two places that ever move
//  g_inputEventReadIndex forward besides PostInputEvent's own dedup path.
//  See src/Window/wininputpump.cpp and docs/targets/InputEventPump.md.
//============================================================================

//  0x6F6C60B0 - drain one event out of the ring.  `__stdcall`, `retn 14h`
//  (five out-pointers on the stack).  Returns 0 if the ring is empty
//  (read index == write index, nothing written); 1 if one event was copied
//  out and the read index advanced, wrapping at the 16th slot.
int __stdcall PopInputEvent(int* outType, int* outA, int* outB, int* outC,
                            int* outD);

//  0x6F6C6130 - overwrite one payload field (0=a, 1=b, 2=c, 3=d - the same
//  indexing PostInputEvent's own argument order gives each field) of every
//  currently-unread ring event, from the read index up to (not including)
//  the write index, with `value`.  `__fastcall(ecx=value, edx=fieldIndex)`,
//  no stack args, plain `retn`.  The one call site this session found
//  (PumpAndPopInputEvent, field 3/"d") stamps the just-drained Win32
//  message's own `time` onto every event still waiting to be popped.
void __fastcall StampPendingInputEventField(int value, int fieldIndex);

//  0x6F6C6720 - the real per-call "give me the next input event" pump.
//  First checks for a pending WM_SIZE/WM_DISPLAYCHANGE lParam
//  (g_lastDisplayChangeParam) and synthesises a type-4 event from its
//  packed width/height if one is waiting.  Otherwise pumps the Win32
//  message queue (PeekMessage/GetMessage/PretranslateWindowMessage/
//  TranslateMessage/DispatchMessage - this is what feeds WndProc and,
//  through it, CGxRawInputBridge/PostInputEvent) until the ring has
//  something to pop: synthesises a type-0xE ("quit") event directly if
//  GetMessage returns 0 (WM_QUIT), or sleeps briefly and returns 0 if the
//  message queue drains with nothing posted.  `__fastcall`: outType in ecx,
//  outA in edx, outB/outC/outD on the stack (`retn 0Ch`).  Returns 1 if
//  *outType was written this call, 0 otherwise.
int __fastcall PumpAndPopInputEvent(int* outType, int* outA, int* outB,
                                    int* outC, int* outD);

//  0x6F6C2810 - the Win32 "accelerator table + modeless-dialog navigation"
//  pretranslate PumpAndPopInputEvent runs every drained message through
//  ahead of TranslateMessage/DispatchMessage: walks every top-level
//  window's own registered accelerator table via TranslateAcceleratorA,
//  then every registered modeless dialog via IsDialogMessageA.  Keyboard-
//  navigation plumbing, not input routing or mouse capture - naked
//  redirect to the real, unreconstructed body; see wininputpump.cpp's own
//  comment on the definition.
int __fastcall PretranslateWindowMessage(const MSG* msg);

//  0x6F6C69A0 - warp the OS cursor to a client-space point on the selected
//  window (GetSelectedWindow(0)), caching it via CacheLastMousePosition
//  first.  `__fastcall(ecx=x, edx=y)`.
void __fastcall WarpCursorTo(int x, int y);

//  0x6F6C69F0 - fill *out with the selected window's client rect as four
//  ints {left,top,right,bottom}, using a cached copy once one exists (the
//  cache is never invalidated anywhere in this call tree).
//  `__fastcall(ecx=out)`.  Returns 0 only if GetClientRect itself fails.
int __fastcall GetCachedClientRect(int* out);

extern int g_shouldSleepForInactivity;   // dword_6FA9FC50
extern int g_inactivitySleepMs;          // dword_6FAD1A94

//  0x6FAD15D8 (IDA's own `Rect`) - {left, top, right, bottom}, the cached
//  selected-window client rect GetCachedClientRect reads and writes.  Never
//  invalidated anywhere in this call tree.
extern int g_cachedClientRect[4];

//  0x6F6CDEB0/0x6F6CDED0 - PumpAndPopInputEvent's own "should I sleep, and
//  for how long" pair, each a bare global read.  Real names, trivial
//  bodies.
int ShouldSleepForInactivity();     // dword_6FA9FC50
int GetInactivitySleepMs();         // dword_6FAD1A94

#endif
