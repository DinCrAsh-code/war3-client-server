//============================================================================
//  Pass-through definitions for functions that reconstructed code declares
//  and calls, but which are not reconstructed yet.
//
//  Without these the tree does not link (LNK1120), so nothing in the repo can
//  be tested in a running game.  Each body jumps to the shipped function at
//  its real address, so an unreconstructed callee simply runs original code.
//
//  Same shape as misc_tailjmp_thunks.cpp / w3gsaction_thunks.cpp, with one
//  difference: those `jmp <symbol>` against a symbol the linker resolves,
//  while these ARE the definitions of those symbols, so they must carry the
//  absolute address themselves.
//
//  NOT reconstructions.  Never offer them as hookable - a detour cannot
//  detour a detour.  Delete each one as its real body lands.
//============================================================================

//----------------------------------------------------------------------------
//  Raw IDA-named callees, referenced by misc_tailjmp_thunks.cpp and
//  deadlocklog_flush.cpp.  The address is the symbol name.
//----------------------------------------------------------------------------

#define RAW_THUNK(name, addr)                     \
    extern "C" __declspec(naked) void name()      \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

RAW_THUNK(sub_6F5FD3B0, 0x6F5FD3B0)
RAW_THUNK(sub_6F5FF380, 0x6F5FF380)
RAW_THUNK(sub_6F7C0CD0, 0x6F7C0CD0)
RAW_THUNK(sub_6F7C2430, 0x6F7C2430)
RAW_THUNK(sub_6F7C24C0, 0x6F7C24C0)
RAW_THUNK(sub_6F7C3B30, 0x6F7C3B30)

//  sub_6F6D8A70/8AD0/94C0/94E0 are reconstructed for real now
//  (debuglock.cpp: DeadlockLog0/1_Init, DeadlockLog0/1_WalkActive) -
//  deadlocklog_flush.cpp calls those instead. sub_6F6D8AC0 stays a thunk on
//  purpose - see critsec.cpp's own note on its ambiguous `lpCriticalSection`
//  operand.
RAW_THUNK(sub_6F6D8AC0, 0x6F6D8AC0)

#undef RAW_THUNK

//  0x6F66D630 - the largest function in ServerTock_0x27_PacketHandler's own
//  dump (381 instructions, a real hash-chain walk under its own SEH frame)
//  and not reconstructed this session - called from
//  servertock_handler.cpp's own root handler as FlushPendingTocks(record,
//  sequence), a genuine __fastcall(ecx, edx) two-register signature per its
//  own call site, not an ordinary thiscall.
//
//  Deliberately NOT `extern "C"`, unlike the RAW_THUNK block above: those
//  carry raw IDA names that other translation units reference from naked
//  asm, so they need C decoration, but this one is called from ordinary C++
//  by name.  servertock_handler.cpp declares it as plain C++
//  (?FlushPendingTocks@@YIXPAXI@Z); an `extern "C"` definition here emits
//  @FlushPendingTocks@8 instead and the two never meet - LNK2019 on every
//  build.  Matching the C++ spelling also means nothing at the call site has
//  to change when the real body lands and this thunk is deleted, which is
//  exactly how the sibling OnTockQueued (actionqueuerecord_notify.cpp) is
//  already spelled.
__declspec(naked) void __fastcall FlushPendingTocks(void*, unsigned int)
{
    __asm { mov eax, 0x6F66D630 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  The five LAN game-list handlers dispatched by W3GSDispatchByType
//  (0x6F663700).  Addresses read off that switch's own call sites in
//  asm/ContainsW3GSBytePacketCheck_0x6F6637C0_calltree_asm.md:
//
//      case 0x2F SearchGame   -> call sub_6F663530
//      case 0x30 GameInfo     -> call sub_6F6625E0
//      case 0x31 CreateGame   -> call sub_6F662E90
//      case 0x32 RefreshGame  -> call sub_6F662EF0
//      case 0x33 DecreateGame -> call sub_6F6626B0
//
//  Declared __stdcall with four arguments to match w3gs_typedispatch.cpp, so
//  the decorated names (_W3GS_SearchGame@16, ...) line up.  A naked body does
//  not touch the stack, so the shipped callee still performs its own ret 16.
//----------------------------------------------------------------------------

#define W3GS_THUNK(name, addr)                                                \
    extern "C" __declspec(naked) void __stdcall name(void*, void*, void*, int)\
    {                                                                         \
        __asm { mov eax, addr }                                               \
        __asm { jmp eax }                                                     \
    }

W3GS_THUNK(W3GS_SearchGame,   0x6F663530)
W3GS_THUNK(W3GS_GameInfo,     0x6F6625E0)
W3GS_THUNK(W3GS_CreateGame,   0x6F662E90)
W3GS_THUNK(W3GS_RefreshGame,  0x6F662EF0)
W3GS_THUNK(W3GS_DecreateGame, 0x6F6626B0)

#undef W3GS_THUNK

//----------------------------------------------------------------------------
//  TaskDescriptor::Dispatch's event handlers (the ItemDrop call chain).  Each
//  address is the one taskdescriptor_dispatch.cpp already records in the
//  comment beside its own declaration, so nothing here is inferred.
//  Every one of them is __stdcall, taking its TaskEventData* on the stack -
//  including the ten that used to be declared __fastcall here.  The dump is
//  unambiguous: sub_6F1026D0 does `push edx` (edx being the event pointer it
//  loaded from its own `[esp+arg_0]`) ahead of every single handler call, and
//  hoists that push above a branch where two calls share it - 0x6F102702
//  covers the 0x6F0F9450/0x6F0F10E0 pair and 0x6F102784 covers the whole
//  0x6F10277F block.  A pushed argument means the callee cleans it.
//
//  **Declaring them __fastcall was a live crash.**  MSVC emitted
//  `call @Handler_X@4` with nothing pushed, so the real, unhooked handler
//  cleaned four bytes that were never there and every call left the stack
//  four bytes short - the "hook pushes less than a real callee cleans"
//  direction in docs/notes/hook-abi-crash-classes.md.  Hooking
//  TaskDescriptor::Dispatch alone crashed the game at eip=0x6660c86, a return
//  through a displaced frame; nothing static flagged it, because the
//  reconstruction's own `ret 4` is correct and only its *outgoing* arity is
//  wrong.  Read the call site, not the callee's prologue.
//----------------------------------------------------------------------------

struct TaskEventData;

#define STDCALL_HANDLER(name, addr)                                  \
    extern "C" __declspec(naked) void __stdcall name(TaskEventData*) \
    {                                                                \
        __asm { mov eax, addr }                                      \
        __asm { jmp eax }                                            \
    }

STDCALL_HANDLER(Handler_PawnGiveDropItemRange, 0x6F0F9450)
STDCALL_HANDLER(Handler_0xD0003,               0x6F0F10E0)
STDCALL_HANDLER(Handler_0xD0022_0xD0027,       0x6F0F1810)
STDCALL_HANDLER(RangeDefaultHandler_0xD0028_0xD017C, 0x6F0F9810)
STDCALL_HANDLER(Handler_0xD017D,               0x6F0F9A20)
STDCALL_HANDLER(Handler_HighRangeDefault,      0x6F0F93F0)
STDCALL_HANDLER(Handler_0xD01A0,               0x6F0DB210)
STDCALL_HANDLER(Handler_0xD017E,               0x6F0FD1B0)
STDCALL_HANDLER(Handler_0xD01A7,               0x6F0DB240)
STDCALL_HANDLER(Handler_AboveD01A7Default,     0x6F0F9B80)
STDCALL_HANDLER(Handler_0xD01AA,               0x6F149E70)

#undef STDCALL_HANDLER

//  0x6F0F17A0 - the one __stdcall in the family, three arguments.
extern "C" __declspec(naked) void __stdcall Handler_0x80261(TaskEventData*, int, int)
{
    __asm { mov eax, 0x6F0F17A0 }
    __asm { jmp eax }
}
//----------------------------------------------------------------------------
//  Unreconstructed C++ members.  Only the mangled name has to match for the
//  link to resolve, and a naked jmp never touches 	his (ecx) or the stack,
//  so the minimal class declarations below need only reproduce the name and
//  signature - not the real layout.  If a declaration here were wrong the
//  symbol would simply stay unresolved; it cannot mis-route a call.
//
//  Addresses are the ones the repo already records, not inferred:
//  funcmap.py for ConditionVariable::Wait, the declaration comments in
//  gameaction0x26_notify.cpp / w3gs_packetcheck.cpp / taskdescriptor_dispatch.cpp
//  for the rest, and threadstarttrampoline.cpp's own header for NetThreadStart.
//----------------------------------------------------------------------------

#define MEMBER_THUNK(cls, method, addr, sig)      \
    class cls { public: void method sig; };       \
    __declspec(naked) void cls::method sig        \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//  ConditionVariable::Wait/NotifyOne's real bodies now live in
//  conditionvariable.h/.cpp - ServerTock_0x27_PacketHandler's own dump
//  reached both (0x6F6D9820 and 0x6F6D8340).

//  sub_6F65AAF0 / sub_6F65AB10 / sub_6F65AB50 / sub_6F65CD70 - w3gs_packetcheck.cpp
class PostStatusTarget
{
public:
    void PostProviderStatus(int);
    void PostProviderStatusWord(int);
    void PostProviderStatusDword(int);
    void ResetRecycleState();
};
__declspec(naked) void PostStatusTarget::PostProviderStatus(int)
{
    __asm { mov eax, 0x6F65AAF0 }
    __asm { jmp eax }
}
__declspec(naked) void PostStatusTarget::PostProviderStatusWord(int)
{
    __asm { mov eax, 0x6F65AB10 }
    __asm { jmp eax }
}
__declspec(naked) void PostStatusTarget::PostProviderStatusDword(int)
{
    __asm { mov eax, 0x6F65AB50 }
    __asm { jmp eax }
}
__declspec(naked) void PostStatusTarget::ResetRecycleState()
{
    __asm { mov eax, 0x6F65CD70 }
    __asm { jmp eax }
}

//  sub_6F479160 - taskdescriptor_dispatch.cpp
class Field154Owner
{
public:
    void Notify(int);
};
__declspec(naked) void Field154Owner::Notify(int)
{
    __asm { mov eax, 0x6F479160 }
    __asm { jmp eax }
}

//  0x6F6E2050 - threadstarttrampoline.cpp's own header comment
class UnidentifiedThreadManager
{
public:
    void NetThreadStart(int, int, int, void*, void*);
};
__declspec(naked) void UnidentifiedThreadManager::NetThreadStart(int, int, int, void*, void*)
{
    __asm { mov eax, 0x6F6E2050 }
    __asm { jmp eax }
}

#undef MEMBER_THUNK
//----------------------------------------------------------------------------
//  The last four.  None of these had an address written down anywhere, so each
//  was recovered from the shipped code rather than guessed. (A fifth,
//  NetProviderBase_Register/0x6F672A60, used to live here the same way -
//  its real body is now in src/Net/netprovider_register.cpp.)
//
//    ActionQueueTable::
//        FindOrCreateRecord    0x6F66CEA0  GameAction_0x26_PacketHandler
//                                          (sub_6F671000) at 0x6F671114, with
//                                          mov ecx, offset stru_6FACFF00
//                                          immediately before it - i.e. called
//                                          on g_actionQueueTable itself
//    ActionQueueRecord::
//        AllocateSlot          0x6F670E90  same handler at 0x6F671190, and the
//                                          address gameaction0x26_handler.cpp
//                                          already names in its comment
//    CDataStoreScratch::
//        WriteRaw              0x6F4C2B20  same handler at 0x6F6711E5, the call
//                                          right after WriteByte (0x6F4C2160)
//                                          and WriteWord (0x6F4C2210), matching
//                                          the source order
//    CDataStoreScratch::
//        OnConstruct           0x6F654210  reached only through the vtable
//                                          (call ds:??_7CDataStoreScratch@@6B@
//                                          at 0x6F6541F1), so it has no direct
//                                          call site.  Read out of the binary:
//                                          the ctor writes the vtable pointer at
//                                          0x6F6541E8 (C7 06 50 0E 97 6F) ->
//                                          vtable 0x6F970E50, whose slot[0] is
//                                          0x6F654210.
//----------------------------------------------------------------------------

//  ActionQueueTable::FindOrCreateRecord's real body now lives in
//  actionqueuetable.cpp - ServerTock_0x27_PacketHandler's own dump reached
//  it.

class ActionQueueRecord
{
public:
    void* __fastcall AllocateSlot(unsigned int, unsigned int);
};
__declspec(naked) void* __fastcall ActionQueueRecord::AllocateSlot(unsigned int, unsigned int)
{
    __asm { mov eax, 0x6F670E90 }
    __asm { jmp eax }
}

//  CDataStoreScratch::OnConstruct used to be thunked here, on the theory that
//  MSVC's COMDAT vtable emission would let this TU's copy fold with the real
//  one instead of colliding.  That is true of the *vtable*, but an
//  out-of-line member function definition is an ordinary strong symbol, so
//  what actually happened was
//
//      unreconstructed_thunks.obj : error LNK2005:
//          "public: virtual void __thiscall CDataStoreScratch::OnConstruct(...)"
//          already defined in cdatastorescratch.obj
//
//  on every link.  Its real body has since landed as an inline definition in
//  cdatastorescratch.h, so per this file's own rule - delete each thunk as
//  its real body arrives - it is gone rather than fixed.
//
//  WriteRaw left the same way earlier: ServerTock_0x27_PacketHandler's own
//  dump reached its real body (sub_6F4C25A0) and its own EnsureWritable
//  callee (sub_6F2C8380), both now in cdatastorescratch.cpp/h.

//  0x6F62CF50 - EvtSched's worker-thread entry point had a forwarding thunk
//  here while its dump was still open worklist.  It is reconstructed now
//  (src/evtworkerthreadmain.cpp, EXACT at 17/17), so the thunk is gone: two
//  definitions of the same symbol is one too many, and the one that wins is
//  whichever the pipeline happens to pick up - which is how this showed up,
//  as EvtWorkerThreadMain scoring 0/17 against a two-instruction listing.

//  0x6F7E18C2 - MSVC8's own __except_handler4.  crtseh4.cpp's __SEH_prolog4
//  pushes its address (that push is at 0x6F7E1C84 in the dump), so the symbol
//  has to exist; nothing in this repo ever calls it.
//
//  A thunk rather than a reconstruction, deliberately.  The real body is only
//  nine instructions and the user read them out of IDA -
//
//      push [esp+10h] / [esp+0Ch] / [esp+8] / [esp+4]
//      push offset SecurityCheckCookie
//      push offset g_securityCookie
//      call _except_handler4_common
//      add  esp, 18h
//      retn
//
//  - but transcribing it would pull in _except_handler4_common, whose address
//  nothing in this repo knows, so it would trade one unresolved external for
//  another rather than getting the link to zero.  Reconstruct it for real
//  once that address is known; the two globals it also needs
//  (SecurityCheckCookie, g_securityCookie) are already mapped.
//
//  `extern "C"`: crtseh4.cpp declares it that way, so the definition has to
//  match (___except_handler4).  That is also why it cannot be bound through
//  funcmap.DATA instead - _readable_from_c_decoration() refuses to guess a
//  readable name for a `__`-prefixed symbol, by design.
extern "C" __declspec(naked) void __except_handler4()
{
    __asm { mov eax, 0x6F7E18C2 }
    __asm { jmp eax }
}
//----------------------------------------------------------------------------
//  0x6F50D970 - hand a sprite's model instance back to whatever owns it.
//  Called by ReleaseSpriteModel (spriteflags.cpp, 0x6F4D3270) with the
//  *model* as `this` and the sprite as its one stack argument.  Below the
//  sub_6F2ACDE0 dump's depth-3 cut, so no body here yet.
//----------------------------------------------------------------------------
#include "sprite.h"

//  0x6F50D970 is a real body now - Widget/modelinstancerelease.cpp, as
//  the free __fastcall ModelInstanceReleaseForSprite.}

//----------------------------------------------------------------------------
//  Callees of the sub_6F2ACDE0 (CWidget::InitSprite) tree that sit below its
//  own depth-3 cut, so no disassembly for them arrived with it.  Ask IDA for
//  their own trees and delete each of these as its real body lands.
//
//  All ordinary C++ spellings, matching their declarations at the call sites
//  exactly - see this file's own FlushPendingTocks note.
//----------------------------------------------------------------------------
#include "tokenizer.h"
#include "widget.h"

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

#include "preferences.h"
#include "playercolor.h"

//  0x6F4E9A50 - CTokenizer's constructor proper is reconstructed for real
//  now, in Storm/tokenizercreate.cpp.
//  0x6F4E9CA0 - one name->id mapping.  Still a thunk: its insert path
//  dispatches through a virtual AllocNode this repo's TSHashTable declares
//  and never defines (see Containers/hashtablectors.h).
ADDR_THUNK(void __fastcall AddToken(CTokenizer*, const char*, int), 0x6F4E9CA0)
//  0x6F005640 is reconstructed for real now, in Config/preferencesctor.cpp.

//  0x6F004390 and 0x6F0057F0 are reconstructed for real now, in
//  Config/preferencecachefind.cpp and Config/preferencestore.cpp; the
//  redirects that stood here are gone, because a redirect left behind after
//  its body lands is a second C++ name for one shipped address and is what
//  verify.py scores instead of the body.  0x6F3A1650 still takes its player
//  in a *stack* slot the callee cleans, not in edx, and is spelled as a
//  thiscall member for that reason; see playercolor.cpp for what the
//  __fastcall spelling does to the stack at run time.
//  0x6F314C80 - SetSpriteColorSlot is reconstructed for real now, in
//  Widget/spritecolorslot.cpp.

//  0x6F32F700 - the sound-cue filter Misc/abilitytablesound.cpp's real
//  callee calls; not itself a worklist entry, left thunked. retn 10h
//  confirmed from its own shipped tail.
ADDR_THUNK(void __fastcall FilterAbilitySoundCue(int, void*, int, int, int,
                                                 int), 0x6F32F700)

#undef ADDR_THUNK

//----------------------------------------------------------------------------
//  The CRT's `atol`, reached by CPreferencesWar3_GetIntValue.  The shipped
//  code calls it indirectly through the import slot (`call ds:__imp_atol`,
//  0x6F86D3E8); this jumps to the import thunk itself (0x6F7E11BC), which is
//  where that slot points once the loader has bound it.  Both addresses read
//  out of the live database via tools/ida_query.py.
//----------------------------------------------------------------------------
__declspec(naked) long __cdecl atol(const char*)
{
    __asm { mov eax, 0x6F7E11BC }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F4D8D60 - CSprite::SetModel, 188 instructions and the largest thing in
//  this dump that is not CSpriteUber::Init.  Deliberately parked rather than
//  reconstructed this session: it is the model-loading path proper (handle
//  resolution, geometry and animation-table construction) rather than the
//  widget-side setup this target is about, and it reaches five more callees
//  below the dump's own depth-3 cut.  Its `funcmap.py` entry and BEHAVIOUR
//  row say so; see docs/targets/CWidget__InitSprite.md.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall SetSpriteModel(CSprite*, unsigned int,
                                                 CTokenizer*, CTokenizer*,
                                                 CTokenizer*, int)
{
    __asm { mov eax, 0x6F4D8D60 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F01F450 - the model-release callback whose address
//  CWar3Image::CreateSpriteFromDescriptor hands to CSprite::LoadModelWithTokens.
//  Never called from this tree, only pointed at.  Below the depth-3 cut.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall ModelReleaseNotify()
{
    __asm { mov eax, 0x6F01F450 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  The sprite-allocation path of the sub_6F2ACDE0 tree, deliberately parked
//  rather than reconstructed this session.
//
//  CreateSpriteForDetailLevel (0x6F4D9830) is a placement-new over
//  CDataAllocator::Alloc under a real C++ EH frame - it installs an
//  `__CxxFrameHandler3` registration and drives an unwind state variable
//  around the constructor call, which this toolchain's fixed `/GS- /EHs-c-`
//  cannot emit at all (docs/msvc-vc8-idioms.md, "An `__except_handler4`-
//  shaped frame this toolchain cannot reproduce").  Its two constructors,
//  CSpriteUber::Init (0x6F4D85D0, 428 instructions) and CSpriteMini::Init
//  (0x6F4D8380), would each need their class's whole vtable modelled before
//  the `mov [esi], offset ??_7CSpriteMini_@@6B@` store could even be
//  spelled.  All three are model-construction rather than the widget-side
//  setup this target is about.
//
//  All three are in verify_known_bad_hooks.json: a redirect thunk patched
//  over its own address is an infinite loop, which tools/abi_audit.py
//  reports as a self-redirect.
//----------------------------------------------------------------------------
__declspec(naked) CSprite* __fastcall CreateSpriteForDetailLevel(int)
{
    __asm { mov eax, 0x6F4D9830 }
    __asm { jmp eax }
}

__declspec(naked) void __fastcall CSpriteUber_Init(void*)
{
    __asm { mov eax, 0x6F4D85D0 }
    __asm { jmp eax }
}

__declspec(naked) void __fastcall CSpriteMini_Init(void*)
{
    __asm { mov eax, 0x6F4D8380 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  The pathing-grid side of CWidget's three footprint slots.  All below the
//  depth cut of their own dumps; ask IDA for their trees and delete these as
//  real bodies land.
//----------------------------------------------------------------------------
#include "widgetfootprint.h"

#define ADDR_THUNK2(sig, addr)                    \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

ADDR_THUNK2(void CFootprintShape::VisitCells(SFootprintPos*, int, void*,
                                             void*), 0x6F3E0D30)
ADDR_THUNK2(void CFootprintShape::CellBox(void*, SFootprintPos*, int),
            0x6F3DB4F0)
ADDR_THUNK2(void __fastcall PathGridInvalidateBox(void*), 0x6F468C70)
//  Address-taken only, never called from here.
ADDR_THUNK2(void __fastcall FootprintMarkCell(), 0x6F2AB0F0)

#undef ADDR_THUNK2

//  0x6F4693D0 / 0x6F2ABEE0 / 0x6F2ADE80 - the pathing-grid region submit and
//  its two address-taken callbacks (widgetfootprintregion.cpp).
__declspec(naked) void __fastcall PathGridSubmitRegion(void*, unsigned int,
                                                       int, int, void*, int*)
{
    __asm { mov eax, 0x6F4693D0 }
    __asm { jmp eax }
}
__declspec(naked) void __fastcall FootprintMarkCellRegion()
{
    __asm { mov eax, 0x6F2ABEE0 }
    __asm { jmp eax }
}
__declspec(naked) void __fastcall FootprintRegionVisit()
{
    __asm { mov eax, 0x6F2ADE80 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  Callees of the five newly surveyed CWidget slots (0xEC, 0xF8, 0xFC,
//  0x120, 0x14C) that sit below their dumps' own depth cut.
//----------------------------------------------------------------------------
#define ADDR_THUNK3(sig, addr)                    \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//  0x6F4A7520 NotifyPathDataChanged is a real body now -
//  Pathfinding/notifypathdatachanged.cpp.
//  0x6F47C100 ReleasePathRegistration is a real body now -
//  Pathfinding/pathref_dtor.cpp.

#undef ADDR_THUNK3

//----------------------------------------------------------------------------
//  CUnit's own not-yet-reconstructed callees (unit.h).  Argument bytes read
//  off each shipped body's `retn`, which is what tools/thunk_abi_audit.py
//  checks and the only thing that stops a wrong declaration corrupting the
//  caller's frame at run time.
//
//    0x6F28DBE0  retn 8   two stack arguments
//    0x6F296C40  retn 4   one
//    0x6F27A5A0  retn 0   none
//    0x6F62A570  retn 8   two (plus `this` in ecx) - reconstructed since,
//                         in Agent/observerpostevent.cpp
//----------------------------------------------------------------------------
#include "unit.h"
#include "observer.h"
#include "widgetregistersubmit.h"
#include "unittrackedref.h"

#define ADDR_THUNK_UNIT(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

ADDR_THUNK_UNIT(void CUnit::RefreshPendingState(int, int), 0x6F28DBE0)
//  0x6F296C40 CUnit::NotifyModelChanged and 0x6F2859F0
//  CUnit::TearDownSelectionVisual are real bodies now -
//  Unit/unit_selectionvisual_notify.cpp.
//  0x6F27A5A0 CUnit::ClearMotionState is a real body now -
//  Unit/unit_clearmotionstate.cpp.
//  0x6F26DE50 CUnit::QueryVisibleImpl is a real body now -
//  Unit/unit_visibilitymask.cpp.
ADDR_THUNK_UNIT(void CUnit::RebuildAnimationIndexTable(int), 0x6F2917B0)
//  CFloat* __fastcall UnitTypeStateValue_6F29C710(CFloat*, unsi ... -> unittypequeries.cpp
//  0x6F3A38F0  retn 14h  five stack arguments plus `this` in ecx.
ADDR_THUNK_UNIT(int SPlayerTable::SubmitUnit(unsigned int, void*, int, int, int), 0x6F3A38F0)

//  SUnitTrackedRef's last un-reconstructed method (unittrackedref.h),
//  `retn 4`.  GetValue (0x6F4773A0) used to be here too and is a real body
//  now - unittrackedref.cpp - along with SetRange (0x6F4774A0).
//  SUnitTrackedRef::SetValue (0x6F477350) used to be here.  Reconstructed
//  with the JASS_KillUnit target - src/Unit/unittrackedref_setvalue.cpp -
//  so the thunk is gone rather than shadowing the real body at link time.

//  0x6F2834E0 / 0x6F40B1E0 - CUnit::IsVisibilityMaskLive's two-step mask
//  lookup.  Both real bodies now, in Unit/unit_visibilitymask.cpp.

//  SUnitMovement's six tail-call targets (unit.h).  Argument bytes off each
//  shipped body's own `retn`; none of them has a dump heading of its own,
//  so tools/thunk_abi_audit.py cannot check these six for us and the
//  counts below were read out of the tail chunks by hand:
//    0x6F474B30 `retn 4`, 0x6F474BA0 `retn 4`, 0x6F474BD0 `retn 4`,
//    0x6F474BF0 `retn 4`, 0x6F476340 `retn 0`, 0x6F477050 `retn 8`.
ADDR_THUNK_UNIT(void SUnitMovement::ApplyClamped(int), 0x6F474B30)
ADDR_THUNK_UNIT(void SUnitMovement::Apply(int), 0x6F474BA0)
ADDR_THUNK_UNIT(void SUnitMovement::SetLowNibble(int), 0x6F474BD0)
ADDR_THUNK_UNIT(void SUnitMovement::SetHighNibble(int), 0x6F474BF0)
ADDR_THUNK_UNIT(int SUnitMovement::GetPlacementMask(), 0x6F476340)
ADDR_THUNK_UNIT(void SUnitMovement::SetPlacementBits(unsigned int, int), 0x6F477050)

//  CUnit's 0x6F29Fxxx type-table probes (unit_placement.cpp) and the two
//  0x6F285xxx helpers slot 97 reaches.  All `retn 0` except where noted.
ADDR_THUNK_UNIT(void __fastcall ComputeUnitTypeValue_6F29DE70(CFloat*, void*, unsigned int), 0x6F29DE70)
ADDR_THUNK_UNIT(int __fastcall UnitTypeFlag_6F29C270(unsigned int), 0x6F29C270)
//  int __fastcall UnitTypeFlag_6F29C2A0(unsigned int) ... -> unittypequeries.cpp
//  void __fastcall UnitTypeRadius_6F29C110(CFloat*, unsigned in ... -> unittypequeries.cpp
//  void __fastcall UnitTypeValue_6F29CBE0(CFloat*, unsigned int ... -> unittypequeries.cpp
ADDR_THUNK_UNIT(unsigned int __fastcall UnitTypeArmorType_6F32D370(unsigned int), 0x6F32D370)

#undef ADDR_THUNK_UNIT

//  0x6F4A9390 (SHandleObjectRange::SetRange) used to be a thunk here.  It
//  is a real body now - handleobjectrange.cpp.

//  0x6F480FB0 (FloatListener::Init) used to be a thunk here.  It is a real
//  body now - src/Unit/floatlistenerinit.cpp, on floatlistener.h's type.
