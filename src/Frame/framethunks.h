//============================================================================
//  Everything the frame library calls that this batch does not reconstruct.
//
//  Two kinds of entry live here and they are not interchangeable:
//
//   * a **declaration of something reconstructed elsewhere** - the Storm
//     ordinals, CDataAllocator, TRefCnt, CObserver - which is included from
//     its own header so that one shipped address keeps one C++ name;
//   * a **naked redirect** to a shipped address whose real body is another
//     batch's, defined in framethunks.cpp.  Each one is a placeholder and
//     nothing more, and each goes away as its own module lands.
//
//  **The declared argument bytes are load-bearing.**  Every callee below
//  cleans its own stack, so a declaration that pushes the wrong number of
//  dwords displaces the caller's frame from the call onwards; the shipped
//  `retn <n>` is written next to every one of them and
//  tools/thunk_abi_audit.py checks it.
//============================================================================
#ifndef FRAMETHUNKS_H
#define FRAMETHUNKS_H

#include "texture.h"

#include "storm.h"
#include "cstatus.h"    //  0x6F4C3E90 & co - Render/
#include "text.h"       //  0x6F4CC470 / 0x6F4CCD70 / 0x6F4CDE70 - Render/

class CFramePoint;
class CLayoutFrame;
class CSimpleFrame;
struct SFrameRect;
class CSimpleTexture;
struct SRenderBatch;

//----------------------------------------------------------------------------
//  Storm.dll, imported by ordinal.  Declared as ordinary C++ and not
//  `extern "C"` for the reason Config/config.h records: the pipeline recovers
//  a readable name from the comment MASM writes beside a mangled EXTRN, and
//  an undecorated C symbol has none.
//----------------------------------------------------------------------------
//  401 and 403 are SMemAlloc/SMemFree and come from Storm/storm.h; 465 is
//  SErrSetLastError and 509 is SStrnICmp, both already named by funcmap - one
//  shipped address, one C++ name.  The rest keep their ordinal names because
//  no published ordinal table this repo has found accounts for them.
void         __stdcall SErrSetLastError(unsigned int error);
void         __stdcall Storm_501(char* dest, const char* src,
                                 unsigned int maxChars);
unsigned int __stdcall Storm_506(const char* s);
char*        __stdcall Storm_507(const char* text, const char* file, int line);
int          __stdcall SStrnICmp(const char* a, const char* b,
                                 unsigned int count);
int                    Storm_578(char* dest, unsigned int size,
                                 const char* format, ...);

//----------------------------------------------------------------------------
//  The 0x6F60xxxx frame modules this batch does not cover.
//----------------------------------------------------------------------------
//  Every entry in this block is a *member* of one of frame.h's classes and
//  is declared there, not here: the shipped call sites leave only `this` in
//  ecx and push everything else, which is __thiscall and not __fastcall, and
//  the difference is visible in the stream (a tail call that rewrites its own
//  argument slot rather than loading edx).  The bodies are the naked
//  redirects in framethunks.cpp:
//
//    CSpriteFrame::ConfigureSprite      0x6F60F1A0  retn 0Ch
//  (CSimpleButton::AttachSlotTexture/0x6F602F80, CSpriteFrame::SetSprite/
//  0x6F60EAA0, SGridRow::SetAlloc/0x6F612830 and SGridRowArray::SetCount/
//  0x6F612D70 are real reconstructions now - Frame/simplebuttonslottexture.cpp,
//  Frame/spriteframesetsprite.cpp and Frame/simplegridrows.cpp.)
//  0x6F609210 / 0x6F6091F0, 0x6F609250 / 0x6F609270, 0x6F60DED0 / 0x6F60DFF0,
//  0x6F60BB70 / 0x6F60BB90, 0x6F60A3E0 / 0x6F60A430 - the element constructor
//  and destructor pairs the vector-constructor iterators run over the five
//  embedded arrays.  All `retn 0`.
//
//  DestroyFrameSlot (0x6F609210) and DestroyFrameLayer (0x6F609270) are real
//  bodies now, in Frame/framedestroyslotlayer.cpp.
void __fastcall ConstructFrameSlot(void* slot);          // 0x6F6091F0
void __fastcall DestroyFrameSlot(void* slot);            // 0x6F609210
void __fastcall ConstructFrameLayer(void* layer);        // 0x6F609250
void __fastcall DestroyFrameLayer(void* layer);          // 0x6F609270
void __fastcall ConstructRenderBatch(void* batch);       // 0x6F60DED0
void __fastcall DestroyRenderBatch(void* batch);         // 0x6F60DFF0
void __fastcall ConstructPriorityArray(void* array);     // 0x6F60BB70
void __fastcall DestroyPriorityArray(void* array);       // 0x6F60BB90
void __fastcall ConstructFrameRef(void* ref);            // 0x6F60A3E0
void __fastcall DestroyFrameRef(void* ref);              // 0x6F60A430
//  0x6F629460 is reconstructed in Frame/framepoint.cpp next to its relative
//  sibling, and frame.h declares it there.

//----------------------------------------------------------------------------
//  This session's own CSimpleButton-vtable closure (docs/targets/vtables/
//  CSimpleButton.txt).  Callees whose own class this session did not need
//  to pin down, so each stays a plain free-function redirect rather than a
//  member of a guessed-at type.
//----------------------------------------------------------------------------
//  0x6F26C350 (`retn 8`, no `this`) - |a-b| < flt_6F92E790 (an epsilon this
//  batch does not otherwise name).  Frame/simpleframevtable0.cpp's own
//  slot-0 body inlines the same test directly rather than calling out to
//  this, which is what its own comment explains.
int __stdcall FloatNearlyEqual(float a, float b);
//  0x6F61C940 (`retn 4`, `this` = CSimpleFrame::m_pOwnedFrame) - push a new
//  rect into the owned frame's own four border-piece sub-rects, weighted by
//  its own +0x38 float divisor.  `this`'s real class is CBackdropGenerator's
//  own border-piece owner, not CSimpleFrame - nothing in this closure
//  reconstructs it, so the redirect takes a bare `void*` as a `__fastcall`
//  first parameter (`edx` is an unused hole - a free function cannot take
//  an explicit `__thiscall` first parameter the way a real member can,
//  docs/msvc-vc8-idioms.md "A receiver in ecx with a second argument in
//  edx is a free `__fastcall`") rather than a guessed member's type.
//  Frame/simpleframevtable0.cpp.
void __fastcall ApplyOwnedFrameRect(void* ownedFrame, void* /*edx, unused*/,
                                    const SFrameRect* rect);
//  0x6F38B200 (`retn 4`) - slot 10's own callee (Frame/simpleframevtable10.cpp):
//  is the 2-float point at `point` inside the 4-float rect at `rect`?  The
//  real body compares point[0] against rect[1]/rect[3] and point[1] against
//  rect[0]/rect[2] - axes swapped from what a {x0,y0,x1,y1}/{x,y} naming
//  would suggest - so this stays an opaque thunk rather than a guessed
//  inline reconstruction that would very likely get an axis backwards.
//  Same free-`__fastcall`-with-a-hole shape as ApplyOwnedFrameRect above.
int __fastcall RectContainsPoint(const float* rect, void* /*edx, unused*/,
                                 const float* point);
//  0x6F60E7E0 / 0x6F60E7C0 (`retn 0`) - hide/show one already-attached
//  CSimpleTexture slot; slot 28's own callees (Frame/simplebuttonvtable28.cpp).
//  Its own +0x7C bit 0 flag, tested/toggled in the shipped body, is what
//  makes each call idempotent.  `texture`'s real class this batch does not
//  otherwise reconstruct, so a plain one-argument `__fastcall` free
//  function (ecx only, no stack argument) rather than a guessed member.
void __fastcall HideSlotTexture(void* texture);
void __fastcall ShowSlotTexture(void* texture);
//  0x6F604FC0 (`retn 4`) - CLayoutFrame::SetRect's own backdrop-rebuild
//  call (Frame/simpleframevtable3.cpp): only reached when this frame has a
//  CBackdropGenerator (m_64 != 0, set by SetOwnedFrame) and unconditionally
//  forwards to sub_6F62B910, the real nine-piece-border geometry -
//  CBackdropGenerator's own module, not this one's.  Declared on
//  CSimpleFrame in frame.h (`this` really is the frame, per the call site).

//----------------------------------------------------------------------------
//  The render, text and sprite engines - other batches' code.
//----------------------------------------------------------------------------
//  CStatus (0x6F4C3E90 and its two helpers) and the font factory
//  (0x6F4CDE70) are reconstructed now, in Render/; cstatus.h and text.h are
//  included above so one shipped address keeps one C++ name.
//  0x6F4D9830 (`retn 0`) - make a sprite for the current detail level.
//  Reconstructed elsewhere (Misc/unreconstructed_thunks.cpp keeps the
//  redirect); re-declared here rather than thunked again, so that one
//  shipped address keeps one C++ name.
class CSprite;
CSprite* __fastcall CreateSpriteForDetailLevel(int detail);
//  0x6F4DE1F0 (`retn 0`) - bit 0 of the texture's +0x10, which for a texture
//  is "does it have an alpha channel".  Misc/trivialpredicates.cpp owns the
//  body and the name.
int   __fastcall GetFlag0x10Bit0_6F4DE1F0(const void* self);
//  0x6F4E3BE0 and 0x6F4E3C80 are reconstructed now, in
//  Render/textureload.cpp and Render/solidtexturemake.cpp;
//  Render/texture.h declares both and this file includes it above.
//  0x6F4E9A80 (`retn 0`) - make a tokenizer over a delimiter set.  Storm's
//  own, reconstructed in Storm/tokenizercreate.cpp.
class CTokenizer;
CTokenizer* __fastcall CreateTokenizer(const char* delimiters);

//----------------------------------------------------------------------------
//  What CSimpleTop, CScreenFrame and CSimpleFontString reach that belongs to
//  batches D, E and F of the JASS_PauseGame closure.  Placeholders, one per
//  address, each of which goes away as its own module lands.
//  0x6F629170 and 0x6F61CD10 are reconstructed now - as
//  CCursorFrame::CCursorFrame (Frame/cursorframector.cpp) and
//  CBackdropGenerator::Generate (Frame/backdropgenerate.cpp), both declared
//  on their real classes in frame.h.  The two placeholder host structs that
//  stood here are gone: a redirect left behind after its body lands is a
//  second C++ name for one shipped address, and verify.py then scores the
//  two-instruction redirect instead of the body.

//  0x6F4CCD70 and 0x6F4CC470 are reconstructed now, in Render/textblockbuild.cpp
//  and Render/textblocktint.cpp; text.h declares both.  BuildTextBlock stopped
//  being `SFontHandle::BuildTextBlock` in the same edit: the shipped function
//  takes the *text* in edx as well as the font in ecx, which a __thiscall
//  member cannot spell, so the seven-stack-argument member declaration that
//  stood here passed the layout engine whatever this build happened to leave
//  in edx.

//  0x6F62B430 - RegisterEventHandler's opposite number, reconstructed in
//  Frame/eventunregister.cpp.  `a` is the value the binding was registered
//  under and `flags` selects which of the three filters
//  RemoveEventHandlers applies; both callers in this closure pass -1.
void __fastcall UnregisterEventHandler(int slot, void* handler, void* a,
                                       int flags);

//  The per-layer shortcut handlers CSimpleTop registers.
//
//  CORRECTED 2026-08-30.  The previous note here said these are "address-taken
//  only - nothing in this closure calls one and no dump carries a body for any
//  of them", and that "all eight are `retn 0`".  Both halves are wrong:
//
//    - 0x6F60C840, inside CSimpleTop::AddToLayerRegions, is a direct
//      `call sub_6F60B410`, and this repo's own
//      asm/sub_6F609B50_0x6F609B50_calltree_asm.md:542 carries that body -
//      90 instructions, listed as a TODO in the matching verify report.
//    - none of them is a `retn 0`.  func_at on all eight, 2026-08-30:
//      B150 0xA4, B200 0xAE, B2B0 0xAE, B360 0xAE, B410 0xEF, B500 0x89,
//      B590 0x85, B620 0x8C bytes, each opening on `push 0FFFFFFFFh`.
//
//  0x6F60B410 is reconstructed now (simpletopmousecapture.cpp); the other
//  seven are still redirects, but redirects to real bodies, not to stubs.
void __fastcall FrameShortcutHandler_6F60B150(void*);
void __fastcall FrameShortcutHandler_6F60B200(void*);
void __fastcall FrameShortcutHandler_6F60B2B0(void*);
void __fastcall FrameShortcutHandler_6F60B360(void*);
//  0x6F60B410 is reconstructed now (simpletopmousecapture.cpp), so this is a
//  real declaration, not a thunk - and the signature the thunk carried was
//  wrong: it takes the raw event in ecx AND the CSimpleTop in edx (the context
//  RegisterEventHandler is handed), and returns int.
int __fastcall FrameShortcutHandler_6F60B410(const void* rawEvent,
                                             class CSimpleTop* top);
void __fastcall FrameShortcutHandler_6F60B500(void*);
void __fastcall FrameShortcutHandler_6F60B590(void*);
void __fastcall FrameShortcutHandler_6F60B620(void*);

//  off_6FA9A410 - the sprite loader's entry point, reached through a global
//  function pointer rather than by name.  The sprite arrives in ecx and the
//  path in edx, with the tokenizer and three zeros on the stack.
extern void (__fastcall * g_pSpriteTokenizeEntry)(void* sprite,
                                                  const char* path,
                                                  void* tokenizer,
                                                  int a, int b, int c);

//----------------------------------------------------------------------------
//  CSimpleTop::Method_0x2C/Method_0x30's own deep callees - real,
//  substantial, dumped in full (asm/sub_6F60C4B0_*.md,
//  asm/sub_6F60C580_*.md) and genuinely out of scope: a Render-module 4x4
//  transform builder several FPU-only calls deep, the same
//  "dependants queue" tree-walk slot 27's own comment already thunks, a
//  qsort comparator whose body was never dumped (only its address is
//  taken), a pooled-object release unrelated to this module, and the
//  888-line Method_0x30 closure's own texture/font leaf.
//----------------------------------------------------------------------------
//  0x6F4E7010 (`retn 4`) - build a transform from `*rect` and `angle`, then
//  do something render-device-side with `originPoint` (CFrame's own
//  on-screen rect corner, at the call site this session covers).  Reaches
//  five further FPU/matrix leaves (sub_6F7B6250, sub_6F4B6170,
//  sub_6F526630/650, sub_6F4B67F0) and the render device's own vtable
//  slots 0x4C/0x50 - none of them reconstructed.
void __fastcall SubmitRegionTransform(const SFrameRect* rect,
                                      const void* originPoint, float angle);

//  0x6F605FE0/0x6F605F50 (`retn 0`, no arguments) - drain the global
//  re-layout queue (g_layoutQueue/dword_6FA9A990, storm.h's own
//  TSExplicitList<CLayoutFrame>, already named in frame.h) one entry at a
//  time; each entry's own removal recurses through its anchor points
//  looking for one whose own +0x34 field is set, and on a hit tail-jumps
//  into that layer's own vtable slot 1 rather than returning normally.
//  The same machinery CFrame::Method_0x6C's own comment already flags as
//  "real but not worth guessing at."
void ProcessLayoutQueue();

//  0x6F7BA980 (`retn 0`) - SRenderBatch::Reset's own resource release: pull
//  `resource` off whichever list currently holds it and push it onto the
//  free list at off_6FAACAC4.  Generic pooled-object plumbing, not this
//  module's own.
void __fastcall ReleaseBatchResource(void* resource);

//----------------------------------------------------------------------------
//  The CPU feature probe, which is not frame code; it is here because
//  CSimpleTop's own module is what reaches it in this call tree.
//----------------------------------------------------------------------------
//  0x6F6CDB60 (`retn 0`, __cdecl) - the raw CPUID wrapper: feature words out
//  through three pointers, and non-zero when CPUID exists at all.
int  CpuIdFeatures(unsigned int* features, unsigned int* extended,
                   unsigned int* vendor);

#endif
