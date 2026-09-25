//============================================================================
//  The real message-pump-and-dispatch layer that sits on top of the raw
//  Win32 input ring (src/Window/wininputbridge.h) - this is the target
//  session's own headline deliverable, see
//  docs/notes/pause-mouse-capture.md's "Claimed... the ring-buffer
//  consumer" section and docs/targets/InputEventPump.md for the full trace.
//
//  PumpAndDispatchInputEvents (0x6F631600) is the drain loop the game's
//  main loop calls once per frame: it repeatedly calls
//  PumpAndPopInputEvent (Window/wininputbridge.h) to pull one event off the
//  ring, and hands each one to DispatchInputEvent (0x6F631490), a 15-way
//  switch on SInputEvent::m_type.  Every branch of that switch re-posts the
//  event, in whatever shape that message type needs, through
//  PostQueuedInputMessage (0x6F632460) - a *different*, Storm-generic,
//  per-object message-subscriber-list broadcast mechanism (confirmed by
//  `xrefs 0x6FACEA68`: its own per-thread registry global has 28 unrelated
//  callers across the binary, not just this module), reached through a
//  per-thread handler object this session did not further identify.  No
//  branch of the switch, and nothing PostQueuedInputMessage's own
//  SEH-guarded body touches, is `CFrame`/`CSimpleTop`-shaped or stores to
//  any object's `+0x16C` - see this file's own targets doc for the honest
//  "does not resolve the investigation, and here is the next edge" writeup.
//
//  Type 9 (mouse-button-down, the path this session followed closest per
//  its own task) is InputMouseDownHandler (0x6F630F60): update the held-
//  button/modifier bitmask, clamp/re-warp the cursor into a global screen-
//  space bound via ClampCursorToScreenBounds, then post message type 0x0B.
//============================================================================
#ifndef INPUTEVENTPUMP_H
#define INPUTEVENTPUMP_H

#include "../GameUI/gameui.h"          // SScreenRect, already reconstructed

//----------------------------------------------------------------------------
//  This module's own globals (0x6FACEB9x-0x6FACEBBx neighbourhood).
//----------------------------------------------------------------------------

//  dword_6FACEBA4 - a live bitmask: mouse-down (InputMouseDownHandler) ORs
//  wParam's own button/modifier bits in; the mouse-up-shaped case (type 13,
//  Method_6F6310A0) ANDs the matching bits back out.  Also the "is anything
//  still held" gate InputEventType0Handler (type 0) spins on with
//  GetTickCount, re-invoking the type-13 handler until it clears.
extern int g_pumpHeldButtonMask;

//  dword_6FACEBA0 - a second bitmask, set/cleared one bit at a time via
//  `1 << (ecx & 3)` in the type-7/type-8 handlers (keyboard modifier
//  down/up, most likely - the shift count is capped at 2, i.e. 3 possible
//  bits) and snapshotted into every posted message's own payload alongside
//  g_pumpHeldButtonMask.
extern int g_pumpModifierBits;

//  dword_6FACEB98 - a small mode selector SampleModifierSnapshot (0/1/2)
//  and several handlers' own posted-message snapshot both read; only the
//  value 1 is ever tested for.
extern int g_pumpInputMode;

//  dword_6FACEBA8/AC - a registered callback and its own context word,
//  invoked by QuitRequestGate (the type-5/type-14 - WM_CLOSE/WM_QUIT -
//  shared handler) when set.  Never written anywhere in this call tree;
//  its installer is out of this session's scope.
typedef int (__fastcall *PumpQuitCallbackFn)(int context);
extern PumpQuitCallbackFn g_pumpQuitCallback;
extern int g_pumpQuitCallbackContext;

//  flt_6FACEBB0 - a global SScreenRect (already reconstructed,
//  GameUI/gameui.h) InputMouseDownHandler's own cursor-clamp math and
//  SScreenRect::Contains both read directly by address.
extern SScreenRect g_pumpClampRect;

//----------------------------------------------------------------------------
//  The pump and its dispatcher.
//----------------------------------------------------------------------------

//  0x6F631600 - `__fastcall(ecx=handler, edx=outQuitRequested)`.  Loops
//  PumpAndPopInputEvent+DispatchInputEvent until a call reports "nothing
//  happened this call" (0); returns 1 if any dispatched event's own
//  quit-shaped branch (type 5 or 14) set *outQuitRequested, else the value
//  it was already carrying in (never itself cleared).  Plain `retn`.
int __fastcall PumpAndDispatchInputEvents(void* handler, int* outQuitRequested);

//  0x6F631490 - the 15-way switch on SInputEvent::m_type (0..14; anything
//  else is a silent no-op).  The real, shipped convention is
//  `__fastcall(ecx=handler, edx=type)` with `eax` carrying a pointer to the
//  event's own four payload words {a,b,c,d} and `esi` - never reloaded at
//  the call site, just left resident from two callers up - carrying
//  PumpAndDispatchInputEvents' own outQuitRequested pointer through to the
//  two branches (type 5/14) that write through it.  That is a genuinely
//  non-standard, whole-program-optimised register argument no MSVC calling
//  convention this toolchain can express places a fourth argument in
//  (CLAUDE.md's own Definition-of-Done section names exactly this class);
//  declared here with an ordinary fourth parameter instead - IDENTICAL by
//  construction on every branch except the two that touch it, which are
//  DIFFERS for this one instruction-level gap.  Plain `retn`.
void __fastcall DispatchInputEvent(void* handler, int type, int* payload,
                                   int* outQuitRequested);

//  0x6F632460 - the generic broadcast: walk the per-thread handler
//  object's own subscriber list for `msgType` (rooted at
//  `handler + msgType*12 + 0x5C`), invoking each registered callback until
//  one returns nonzero.  `__fastcall(ecx=handler, edx=msgType)` plus one
//  stack pointer to the message's own payload (`retn 4`).  SEH-guarded,
//  Storm-generic infrastructure with 28 unrelated call sites elsewhere in
//  the binary (`xrefs 0x6FACEA68`) - naked redirect, out of this session's
//  scope; see docs/targets/InputEventPump.md.
void __fastcall PostQueuedInputMessage(void* handler, int msgType,
                                       const void* payload);

//  0x6F630DB0 - the type-5/type-14 (WM_CLOSE/WM_QUIT-derived) shared
//  handler: if g_pumpQuitCallback is set, tail-calls it with
//  g_pumpQuitCallbackContext in ecx and returns whatever it returns;
//  otherwise returns 1.  Plain `retn`, no stack args.
int QuitRequestGate();

//  0x6F630EB0 - QuitRequestGate's own "and then run the real quit-signal
//  broadcast" follow-up (DispatchInputEvent's case 5/14, after
//  QuitRequestGate returns nonzero).  Naked redirect to the real,
//  unreconstructed body (a further tail-chain into Storm's own generic
//  per-thread-queue wake-up, `SetEvent` on a registered handle list) - out
//  of scope, unrelated to mouse routing.
void QuitBroadcastRelay();

//  0x6F631200 - reset both bitmasks to 0.  Plain `retn`.
void ResetPumpAccumulators();

//  0x6F630B80 - a "did the held-modifier mask actually change under a
//  watched bit" gate, tail-calling into the same stru_6FACEA68-keyed
//  registry cluster PostQueuedInputMessage's own handler object comes from
//  (a different member function of it, `sub_6F62B7E0`) when it did.  Naked
//  redirect - same generic, out-of-scope infrastructure as
//  PostQueuedInputMessage itself.
void PumpModifierChangeGate();

//----------------------------------------------------------------------------
//  The 13 non-mouse-down case handlers, named by dispatch type.  Each posts
//  through PostQueuedInputMessage with its own message-type constant and
//  payload shape - see inputeventhandlers.cpp for the per-case detail.
//
//  Several of these receive `a` (the event's own first payload word) or a
//  count through `eax`/`edx` ahead of `handler` on the stack - the same
//  non-standard, whole-program-optimised "extra register argument" shape
//  CacheLastMousePosition/MapButtonMessageToBit already carry a `DIFFERS`
//  BEHAVIOUR entry for (Window/winmousehelpers.cpp's own header comment).
//  Declared here with `handler` and every payload word as ordinary stack
//  parameters instead; each carries the same kind of BEHAVIOUR entry where
//  the diff lands on that one instruction-level gap rather than a real one.
//----------------------------------------------------------------------------
void __stdcall InputEventType0Handler(void* handler, int b, int c);
void __stdcall InputEventType1Handler(void* handler, int a, int b);
void __stdcall InputEventType2Handler(void* handler, int a, int count);
void __stdcall InputEventType3Handler(void* handler, int a, int b, int c);
void __stdcall InputEventType4Handler(void* handler, int a, int b);
void __stdcall InputEventType6Handler(void* handler, int a);
void __stdcall InputEventType7Handler(void* handler, int a, int b, int c);
void __stdcall InputEventType8Handler(void* handler, int a, int b, int c);
void __stdcall InputEventType10Handler(void* handler, int b, int c);
void __stdcall InputEventType11Handler(void* handler, int a, int b, int c);
void __stdcall InputEventType12Handler(void* handler, int b, int c);
void __stdcall InputEventType13Handler(void* handler, int a, int b, int c,
                                       int d);

//----------------------------------------------------------------------------
//  ClampCursorToScreenBounds (0x6F630C20) - see inputcursorclamp.cpp's own
//  header comment.
//----------------------------------------------------------------------------
void __fastcall ClampCursorToScreenBounds(int x, int y, float* outFracX,
                                          float* outFracY);

//  0x6F630F60 - the type-9 (mouse-button-down) handler.  `__fastcall`,
//  `retn 10h` (four stack dwords: b, c, d, handler).  See
//  inputmousedown.cpp's own header comment.
void __fastcall InputMouseDownHandler(int wParam, void* handler, int x,
                                      int y, int d);

//  0x6F630D90 - a small mode-gate every handler that posts a "snapshot"
//  payload calls first: returns 2 if g_pumpInputMode == 1, else 0.  Plain
//  `retn`, no args (reads the global directly).
int SampleModifierSnapshot();

#endif
