//============================================================================
//  See framethunks.h.  One naked redirect per shipped address; nothing in
//  this file is a reconstruction, and every one of them goes away as its own
//  module of the frame library lands.
//
//  Spelled as a *trailing* macro so tools/thunk_inventory.py counts every one
//  of them: its CALL_THUNK pattern wants `NAME_THUNK(0x<addr>)` after the
//  signature.
//============================================================================
#include "framethunks.h"
#include "frame.h"
#include "framereg.h"
#include "mouseevent.h"

#define FRAME_THUNK(addr)        \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

//  --- the frame modules this batch does not cover ---------------------------
//  The __thiscall members.  Each is declared on its own class in frame.h;
//  the shipped call sites leave only `this` in ecx, which is what makes them
//  members rather than __fastcall free functions.
//
//  0x6F602F80 - CSimpleButton::AttachSlotTexture is a real body now, in
//  Frame/simplebuttonslottexture.cpp.
//  0x6F60EAA0 - CSpriteFrame::SetSprite is a real reconstruction now,
//  Frame/spriteframesetsprite.cpp; the redirects that stood in for both
//  here are gone with them.

__declspec(naked) void CSpriteFrame::ConfigureSprite(void*, int, int) FRAME_THUNK(0x6F60F1A0)

//  0x6F612830 / 0x6F612D70 are real reconstructions now,
//  Frame/simplegridrows.cpp; the redirects that stood in for them here are
//  gone with them.

__declspec(naked) void __fastcall ConstructFrameSlot(void*) FRAME_THUNK(0x6F6091F0)
__declspec(naked) void __fastcall ConstructFrameLayer(void*) FRAME_THUNK(0x6F609250)
//  DestroyFrameSlot (0x6F609210) / DestroyFrameLayer (0x6F609270) are real
//  bodies now, in Frame/framedestroyslotlayer.cpp.
__declspec(naked) void __fastcall ConstructRenderBatch(void*) FRAME_THUNK(0x6F60DED0)
__declspec(naked) void __fastcall DestroyRenderBatch(void*) FRAME_THUNK(0x6F60DFF0)
__declspec(naked) void __fastcall ConstructPriorityArray(void*) FRAME_THUNK(0x6F60BB70)
__declspec(naked) void __fastcall DestroyPriorityArray(void*) FRAME_THUNK(0x6F60BB90)
__declspec(naked) void __fastcall ConstructFrameRef(void*) FRAME_THUNK(0x6F60A3E0)
__declspec(naked) void __fastcall DestroyFrameRef(void*) FRAME_THUNK(0x6F60A430)

//  0x6F5C94D0 is reconstructed now - Frame/framedefcreateinstance.cpp - and
//  the redirect that stood in for it here is gone with it.  Its own two
//  callees are not, and are declared in framereg.h.
struct BASEFRAMEHASHNODE;
class CSimpleFrame;
typedef CSimpleFrame* (__fastcall *SimpleFrameCreateFn)(CSimpleFrame*);
SimpleFrameCreateFn __fastcall ResolveSimpleFrameCreateHandler(const char*,
                                                               void*, void**);
//  0x6F5CCC80 - out of this batch's own dump (confirmed by
//  CreateSimpleFrameFromDef's own xrefs, `worktree_store.py xrefs
//  0x6F5C94D0 --from`); link_check.py caught this as declared and called
//  but never defined anywhere - a naked redirect until its own dump lands.
__declspec(naked) void __fastcall ApplyDefToSimpleFrame(BASEFRAMEHASHNODE*, void*,
                                      CSimpleFrame*) FRAME_THUNK(0x6F5CCC80)
//  0x6F5C9420 - the frame path's create-from-definition.  `retn 10h`: the
//  definition in ecx, the parent in edx and four dwords pushed.  It is not a
//  reconstruction and cannot become one: it hands its chain-list argument to
//  0x6F5C8E10 in *eax* (`mov eax, ebx` / `call`), and the callee opens
//  `mov eax, [eax+8]` - a whole-program register convention no MSVC spelling
//  asks for.
__declspec(naked) CFrame* __fastcall CreateFrameFromDef(
        BASEFRAMEHASHNODE*, CFrame*, void*, void*, void*,
        STabChainList*) FRAME_THUNK(0x6F5C9420)

//  0x6F5C93E0 / 0x6F5C93A0 - real bodies now, in
//  Frame/framecreatehandlerresolve.cpp.
//  0x6F5CCC80 - real body now, in Frame/framedefapplysimple.cpp.

//  --- the render, text and sprite engines -----------------------------------
//  0x6F4C3660 - CStatus::IsEmpty is reconstructed for real now, in
//  Render/statusisempty.cpp.
//  0x6F4C3A00 - `retn 4`.  Still a redirect: BuildText's join walk calls two
//  further undumped helpers (0x6F4C39D0, a length pass; 0x6F4C3980, the
//  actual copy) that would have to be guessed rather than read.
__declspec(naked) char* CStatus::BuildText(int) FRAME_THUNK(0x6F4C3A00)
//  0x6F4CBF60 is a real body, PostSysMessage in Misc/sysmessagepost.cpp.  The
//  redirect that used to sit here was declared `ShowSysMessageText`, a second
//  C++ name for that one shipped address, and it shadowed the real body for
//  every caller that used the alias.

//  CStatus's destructor (0x6F4C3E90) and the font factory (0x6F4CDE70) are
//  real bodies now, in Render/statusdtor.cpp and Render/fontcreate.cpp.

//  0x6F4E3BE0 and 0x6F4E3C80 are real bodies now, in Render/textureload.cpp
//  and Render/solidtexturemake.cpp, and Render/texture.h declares both.
//  Leaving the redirects here would be a second C++ name for each shipped
//  address, and verify.py would score the two-instruction redirect rather
//  than the body - which is exactly what it did for one run.

//  --- the CPU feature probe -------------------------------------------------
__declspec(naked) int CpuIdFeatures(unsigned int*, unsigned int*,
                                    unsigned int*) FRAME_THUNK(0x6F6CDB60)

//  --- batches D, E and F ----------------------------------------------------
//  0x6F4CCD70 and 0x6F4CC470 are real bodies now, in Render/textblockbuild.cpp
//  and Render/textblocktint.cpp.

//  The eight shortcut handlers, address-taken only.  All `retn 0`.
__declspec(naked) void __fastcall FrameShortcutHandler_6F60B150(void*) FRAME_THUNK(0x6F60B150)
__declspec(naked) void __fastcall FrameShortcutHandler_6F60B200(void*) FRAME_THUNK(0x6F60B200)
__declspec(naked) void __fastcall FrameShortcutHandler_6F60B2B0(void*) FRAME_THUNK(0x6F60B2B0)
__declspec(naked) void __fastcall FrameShortcutHandler_6F60B360(void*) FRAME_THUNK(0x6F60B360)
//  0x6F60B410 is no longer a thunk - it is reconstructed in
//  simpletopmousecapture.cpp.  Note the comment above is wrong for the other
//  seven as well: none of them is a `retn 0`, they are 0x85-0xEF byte
//  SEH-framed functions (func_at on all eight, 2026-08-29), and this one is
//  called directly from AddToLayerRegions at 0x6F60C840.
__declspec(naked) CMouseEvent::CMouseEvent(const void*) FRAME_THUNK(0x6F2F5920)
__declspec(naked) void __fastcall FrameShortcutHandler_6F60B500(void*) FRAME_THUNK(0x6F60B500)
__declspec(naked) void __fastcall FrameShortcutHandler_6F60B590(void*) FRAME_THUNK(0x6F60B590)
__declspec(naked) void __fastcall FrameShortcutHandler_6F60B620(void*) FRAME_THUNK(0x6F60B620)

//----------------------------------------------------------------------------
//  The embedded array elements whose own constructors are other modules'
//  work.  Each is a real C++ constructor - that is what makes MSVC emit the
//  `eh vector constructor iterator` the shipped code has - whose body is one
//  call into the shipped body.
//
//  CLayoutFrame's, CLayer's and CSimpleRegion's constructors used to live
//  here as redirects too.  All three are real reconstructions now
//  (clayoutframector.cpp, clayerctor.cpp, simpleregionctor.cpp), and each of
//  those addresses carries one C++ name again.
//----------------------------------------------------------------------------
SFrameSlot::SFrameSlot()      { ConstructFrameSlot(this); }
SFrameSlot::~SFrameSlot()     { DestroyFrameSlot(this); }
SFrameLayer::SFrameLayer()    { ConstructFrameLayer(this); }
SFrameLayer::~SFrameLayer()   { DestroyFrameLayer(this); }
SRenderBatch::SRenderBatch()  { ConstructRenderBatch(this); }
SRenderBatch::~SRenderBatch() { DestroyRenderBatch(this); }
CFramePriorityArray::CFramePriorityArray()  { ConstructPriorityArray(this); }
CFramePriorityArray::~CFramePriorityArray() { DestroyPriorityArray(this); }
SFrameRef::SFrameRef()        { ConstructFrameRef(this); }
SFrameRef::~SFrameRef()       { DestroyFrameRef(this); }

//  0x6F601DE0 - CControl::SetStateBits.  Twelve instructions and a tail jump
//  into slot 60; another module's, so a redirect for now.
//  0x6F607C30 - CFrame::AddFog.  A dedup-then-splice shape more involved
//  than AddLight's own (see framelight.cpp) - a first attempt at a real
//  body scored only 0.329, so it stays a redirect pending a session with
//  the budget to read the diff properly rather than guess again.
__declspec(naked) void CFrame::AddFog(void*, int) FRAME_THUNK(0x6F607C30)

__declspec(naked) void CControl::SetStateBits(int) FRAME_THUNK(0x6F601DE0)

//----------------------------------------------------------------------------
//  This session's own CSimpleButton-vtable closure - see both declarations'
//  own comments in frame.h / framethunks.h.
//----------------------------------------------------------------------------
__declspec(naked) void CLayoutFrame::RecomputeQueuedLayout(const SFrameRect*) FRAME_THUNK(0x6F605DD0)
__declspec(naked) int CLayoutFrame::ComputeContentRect(SFrameRect*) FRAME_THUNK(0x6F605800)
__declspec(naked) void CSimpleFrame::RecomputeScreenRect(const SFrameRect*) FRAME_THUNK(0x6F6085E0)
__declspec(naked) int __stdcall FloatNearlyEqual(float, float) FRAME_THUNK(0x6F26C350)
__declspec(naked) void __fastcall ApplyOwnedFrameRect(void*, void*, const SFrameRect*) FRAME_THUNK(0x6F61C940)
__declspec(naked) int __fastcall RectContainsPoint(const float*, void*, const float*) FRAME_THUNK(0x6F38B200)
__declspec(naked) void __fastcall HideSlotTexture(void*) FRAME_THUNK(0x6F60E7E0)
__declspec(naked) void __fastcall ShowSlotTexture(void*) FRAME_THUNK(0x6F60E7C0)
__declspec(naked) int CSimpleFrame::Method_0x3C(int, void*) FRAME_THUNK(0x6F609C00)
__declspec(naked) void CSimpleTop::RemoveFromLayerRegions(CSimpleFrame*) FRAME_THUNK(0x6F60C3B0)
__declspec(naked) void CSimpleTop::AddToLayerRegions(CSimpleFrame*, int) FRAME_THUNK(0x6F60C760)
__declspec(naked) void CSimpleButton::RealDestructorBody() FRAME_THUNK(0x6F603800)
__declspec(naked) void CSimpleButton::UpdateVisualForState(int) FRAME_THUNK(0x6F6034C0)
__declspec(naked) void CSimpleFrame::RebuildBackdropRect(int) FRAME_THUNK(0x6F604FC0)

//  CFrame's own vtable slots 9, 46, 47, 52, 53, 56 (docs/targets/vtables/
//  CSimpleTop.txt) - real, shared bodies (same address on a fresh
//  CSimpleTop and CWorldFrameWar3 pull) this session left naked thunks:
//  every one is real per-frame coordinate/rect geometry through this
//  frame's own children and dependants, each pulling in several more
//  `flt_6FACE6xx` globals and further indirect vtable calls, and none of
//  them is CSimpleTop-specific (CWorldFrameWar3 carries the identical
//  address at each), so getting them exactly right was not this session's
//  priority once the class's own 11 slots and the tractable shared ones
//  were covered.
__declspec(naked) void CFrame::Method_0x24(void*)        FRAME_THUNK(0x6F5FD650)
__declspec(naked) void CFrame::Method_0xB8(void*, void*) FRAME_THUNK(0x6F606BD0)
__declspec(naked) void CFrame::Method_0xBC(void*, void*, void*) FRAME_THUNK(0x6F606CF0)
__declspec(naked) void CFrame::Method_0xD0()              FRAME_THUNK(0x6F5FE6F0)
__declspec(naked) void CFrame::Method_0xD4()              FRAME_THUNK(0x6F5FE690)
__declspec(naked) void CFrame::Method_0xE0(float*)         FRAME_THUNK(0x6F6071A0)

//  CSimpleTop::Method_0x2C/Method_0x30's own deep callees - see
//  framethunks.h's own comment above each declaration.
__declspec(naked) void __fastcall SubmitRegionTransform(const SFrameRect*,
    const void*, float)                                    FRAME_THUNK(0x6F4E7010)
__declspec(naked) void ProcessLayoutQueue()                FRAME_THUNK(0x6F605FE0)
__declspec(naked) void SDirtyBatchContext::ProcessDirtyRenderBatch(SRenderBatch*)
                                                            FRAME_THUNK(0x6F60D1B0)
__declspec(naked) void __fastcall ReleaseBatchResource(void*) FRAME_THUNK(0x6F7BA980)
__declspec(naked) void SBatchPtrArray::EnsureCapacity(unsigned int) FRAME_THUNK(0x6F60AF30)

#undef FRAME_THUNK
