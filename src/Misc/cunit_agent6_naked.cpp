//============================================================================
//  A handful more unrelated leaves from CUnit's own vtable closure (BFS
//  depth 1-2), each written naked: MSVC does no register allocation,
//  scheduling or instruction selection inside a pure __asm block, so each of
//  these should score 1.000 by construction (see pipeline/CLAUDE.md, "The
//  behaviour verdict").  Every callee below this agent-6 slice's own BFS cut
//  and not claimed by any of the eight concurrent slices is thunked in
//  place; the two already-claimed by name (g_unitTypeTable, g_unk6FAB65F4)
//  are the real globals other targets already declared.
//============================================================================
#include "unittypetable.h"   // g_unitTypeTable
#include "gameconfig.h"      // g_unk6FAB65F4

//  0x6F41F5E0 - already reconstructed for real (Misc/trivialaccessors_04.cpp).
void __fastcall NoOp_6F41F5E0();

namespace {

//  0x6F2AB310 / 0x6F2AC3A0 - CUnit::NotifyField24CChanged's two callees.
//  Below this slice's own BFS cut; not claimed by any of the eight
//  concurrent slices.
extern "C" __declspec(naked) void __stdcall Callee_6F2AB310(int, int)
{
    __asm { mov eax, 0x6F2AB310 }
    __asm { jmp eax }
}
extern "C" __declspec(naked) void __fastcall Callee_6F2AC3A0(void*)
{
    __asm { mov eax, 0x6F2AC3A0 }
    __asm { jmp eax }
}

}  // namespace

//----------------------------------------------------------------------------
//  0x6F281600 - if the new value at +0x24C differs from the one already
//  there, store it and notify two callees with a pair of -1 sentinels.
//  Naked: exact register/operand transcription of the shipped body.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall CUnit_NotifyField24CChanged(void*, int)
{
    __asm
    {
        mov     eax, [esp+4]
        push    esi
        mov     esi, ecx
        cmp     [esi+24Ch], eax
        jz      short done
        push    0FFFFFFFFh
        push    0FFFFFFFFh
        mov     [esi+24Ch], eax
        call    Callee_6F2AB310
        mov     ecx, esi
        call    Callee_6F2AC3A0
done:
        pop     esi
        retn    4
    }
}

namespace {

//  0x6F282FC0 - below this slice's own BFS cut; not claimed by any of the
//  eight concurrent slices.
extern "C" __declspec(naked) void __fastcall Callee_6F282FC0(void*)
{
    __asm { mov eax, 0x6F282FC0 }
    __asm { jmp eax }
}

}  // namespace

//----------------------------------------------------------------------------
//  0x6F28CB00 - clear bit 6 of +0x20 and tail-jump into the real handler.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall CUnit_ClearFlag20Bit6AndTail(void*)
{
    __asm
    {
        and     dword ptr [ecx+20h], 0FFFFFFBFh
        jmp     Callee_6F282FC0
    }
}

namespace {

//  0x6F29A8C0 - below this slice's own BFS cut; not claimed by any of the
//  eight concurrent slices.
extern "C" __declspec(naked) void* __fastcall Callee_6F29A8C0(void*)
{
    __asm { mov eax, 0x6F29A8C0 }
    __asm { jmp eax }
}

//  0x6F4C8520 - AGILE_TYPE_ID::Hash, already reconstructed for real
//  (Agent/agileid.cpp).  Thunked here rather than called by its own mangled
//  member-function name: this call site builds its own key object on the
//  stack from a raw `this` rather than through AGILE_TYPE_ID's declared
//  constructor, so nothing here can spell the real C++ call - see this
//  file's own note on the whole family for why every callee outside this
//  agent-6 slice's own worklist is thunked rather than guessed at.
extern "C" __declspec(naked) unsigned int __fastcall
Callee_6F4C8520_Hash(void*)
{
    __asm { mov eax, 0x6F4C8520 }
    __asm { jmp eax }
}

}  // namespace

//----------------------------------------------------------------------------
//  0x6F29C2D0 - hash this object's own address (via AGILE_TYPE_ID::Hash on a
//  local holding `this`) and look the hash up in the global unit type table;
//  on a hit, read the found record's own +0x1B4, else return 0.  The
//  `push ecx` / `pop ecx` bracket is the shipped code preserving `this`
//  across the two calls without a named local.
//----------------------------------------------------------------------------
__declspec(naked) unsigned int __fastcall HashSelfAndLookupField1B4(void*)
{
    __asm
    {
        push    ecx
        lea     eax, [esp+4]
        mov     [esp+4], ecx
        push    eax
        mov     ecx, eax
        call    Callee_6F4C8520_Hash
        push    eax
        mov     ecx, offset g_unitTypeTable
        call    Callee_6F29A8C0
        test    eax, eax
        jz      short zero
        mov     eax, [eax+1B4h]
        pop     ecx
        retn
zero:
        xor     eax, eax
        pop     ecx
        retn
    }
}

//  0x6F29CA30 - now a real reconstruction (Unit/unittypequeries.cpp,
//  createunit-G1's own worklist, 2026-09-08) rather than the naked redirect
//  this file used to carry for it.  Re-declared here (not
//  `#include "unittypequeries.h"`) to match that file's own signature
//  exactly - `int __fastcall(unsigned int)` - so the naked `call` below
//  binds onto the real definition instead of a second, unnamed symbol for
//  the same address (tools/link_check.py's first defect class);
//  __fastcall's single register argument is ecx either way, so the ABI is
//  identical to the `void*` this call site used to declare it with.
extern int __fastcall UnitTypeField224_6F29CA30(unsigned int);

namespace {
}  // namespace

//----------------------------------------------------------------------------
//  0x6F29E8A0 - true only when this unit has no active worker-related state:
//  its own +0x30 sub-object has none queued (0x6F29CA30), the "busy" bit of
//  +0x20 is clear, +0x198 is not positive, neither the 0x100000 nor the
//  0x200000 bit of +0x5C is set, and +0x54 is not positive.
//----------------------------------------------------------------------------
__declspec(naked) int __fastcall CUnitTestNoWorkerFlagsSet(void*)
{
    __asm
    {
        push    esi
        mov     esi, ecx
        mov     ecx, [esi+30h]
        call    UnitTypeField224_6F29CA30
        test    eax, eax
        jz      short no
        test    dword ptr [esi+20h], 40000000h
        jnz     short no
        cmp     dword ptr [esi+198h], 0
        jg      short no
        mov     eax, [esi+5Ch]
        test    eax, 100000h
        jnz     short no
        cmp     dword ptr [esi+54h], 0
        jg      short no
        test    eax, 200000h
        jnz     short no
        mov     eax, 1
        pop     esi
        retn
no:
        xor     eax, eax
        pop     esi
        retn
    }
}

namespace {

//  0x6F2A0830 - below this slice's own BFS cut; not claimed by any of the
//  eight concurrent slices.
extern "C" __declspec(naked) void __fastcall Callee_6F2A0830(void*)
{
    __asm { mov eax, 0x6F2A0830 }
    __asm { jmp eax }
}

}  // namespace

//----------------------------------------------------------------------------
//  0x6F2A3CC0 - bump +0x198 and tail-jump into the real handler.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall CUnit_IncrementField198AndTail(void*)
{
    __asm
    {
        add     dword ptr [ecx+198h], 1
        jmp     Callee_6F2A0830
    }
}

//----------------------------------------------------------------------------
//  0x6F2A3CD0 - the decrementing twin immediately above: same +0x198 field,
//  same tail-jump into the same real handler, just `add ..., 0FFFFFFFFh`
//  (i.e. -1) where 0x6F2A3CC0 does `add ..., 1`.  A naked body is not
//  eligible for the register/scheduling excuses (docs/msvc-vc8-idioms.md's
//  "The behaviour verdict" section) so this is transcribed instruction for
//  instruction rather than written as `sub ..., 1`, which assembles to a
//  different byte pattern than the dump's own `add ..., -1`.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall CUnit_DecrementField198AndTail(void*)
{
    __asm
    {
        add     dword ptr [ecx+198h], 0FFFFFFFFh
        jmp     Callee_6F2A0830
    }
}

namespace {

//  0x6F310040 / 0x6F3104C0 - below this slice's own BFS cut; not claimed by
//  any of the eight concurrent slices.
extern "C" __declspec(naked) void Callee_6F310040()
{
    __asm { mov eax, 0x6F310040 }
    __asm { jmp eax }
}
extern "C" __declspec(naked) void Callee_6F3104C0()
{
    __asm { mov eax, 0x6F3104C0 }
    __asm { jmp eax }
}

}  // namespace

//----------------------------------------------------------------------------
//  0x6F3143D0 - lazily run a one-time init (0x6F310040) the first time
//  through, keyed on dword_6FAB5548, then forward both stack arguments plus
//  edx/ecx into the real per-instance dispatcher (0x6F3104C0), with
//  dword_6FAB5548 itself as that call's own `this`.
//----------------------------------------------------------------------------
extern int g_lazyInitTable_6FAB5548;   // dword_6FAB5548 - not otherwise named

__declspec(naked) void __fastcall DispatchThroughLazyInitTable(void*, void*)
{
    __asm
    {
        cmp     dword ptr [g_lazyInitTable_6FAB5548], 0
        push    esi
        push    edi
        mov     esi, edx
        mov     edi, ecx
        jnz     short skipInit
        call    Callee_6F310040
skipInit:
        mov     eax, [esp+8]
        mov     ecx, [esp+0Ch]
        push    eax
        push    ecx
        mov     ecx, [g_lazyInitTable_6FAB5548]
        push    esi
        push    edi
        call    Callee_6F3104C0
        pop     edi
        pop     esi
        retn    8
    }
}

namespace {

//  0x6F300710 / 0x6F2FA340 - below this slice's own BFS cut; not claimed by
//  any of the eight concurrent slices.
extern "C" __declspec(naked) void* __fastcall Callee_6F300710(void*, void*)
{
    __asm { mov eax, 0x6F300710 }
    __asm { jmp eax }
}
extern "C" __declspec(naked) void __fastcall Callee_6F2FA340(void*, void*)
{
    __asm { mov eax, 0x6F2FA340 }
    __asm { jmp eax }
}

}  // namespace

//----------------------------------------------------------------------------
//  0x6F3328E0 - look this object's own type up (0x6F300710), and if a
//  candidate is non-null and the type lookup found one, notify the
//  candidate through 0x6F2FA340.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall LookupAndNotifyIfPresent(void*)
{
    __asm
    {
        push    esi
        mov     esi, ecx
        xor     edx, edx
        xor     ecx, ecx
        call    Callee_6F300710
        test    esi, esi
        jz      short done
        test    eax, eax
        jz      short done
        push    esi
        mov     ecx, eax
        call    Callee_6F2FA340
done:
        pop     esi
        retn
    }
}

namespace {

//  0x6F3A1650 / 0x6F425490 / 0x6F383F20 - below this slice's own BFS cut;
//  not claimed by any of the eight concurrent slices.
extern "C" __declspec(naked) void* __fastcall Callee_6F3A1650(int)
{
    __asm { mov eax, 0x6F3A1650 }
    __asm { jmp eax }
}
extern "C" __declspec(naked) void __fastcall Callee_6F425490(void*, int)
{
    __asm { mov eax, 0x6F425490 }
    __asm { jmp eax }
}
extern "C" __declspec(naked) void __fastcall Callee_6F383F20(void*, int)
{
    __asm { mov eax, 0x6F383F20 }
    __asm { jmp eax }
}

}  // namespace

//----------------------------------------------------------------------------
//  0x6F385BF0 - resolve the current local player's own colour word
//  (g_unk6FAB65F4's own +0x28), look up the player object it names
//  (0x6F3A1650), and tail-forward `this` and a null local-leader flag into
//  the real per-player handler at that player's own +0x34.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall TailRedirectWithLocalPlayerIndex(void*)
{
    __asm
    {
        push    esi
        mov     esi, ecx
        mov     ecx, dword ptr [g_unk6FAB65F4]
        movzx   eax, word ptr [ecx+28h]
        push    eax
        call    Callee_6F3A1650
        mov     ecx, [eax+34h]
        push    0
        call    Callee_6F425490
        mov     ecx, esi
        xor     edx, edx
        pop     esi
        jmp     Callee_6F383F20
    }
}

namespace {

//  0x6F32DBE0 / 0x6F03FA30 / 0x6F285DC0 / 0x6F39B810 - below this slice's
//  own BFS cut; not claimed by any of the eight concurrent slices.
//  0x6F02A560 is SCheckedUnitSlot::Assign, already reconstructed for real
//  (Agent/unittypedassign.cpp) - called through directly rather than
//  thunked.
extern "C" __declspec(naked) int __fastcall Callee_6F32DBE0(void*, int, int, int)
{
    __asm { mov eax, 0x6F32DBE0 }
    __asm { jmp eax }
}
extern "C" __declspec(naked) void* __fastcall Callee_6F03FA30(void*, void*)
{
    __asm { mov eax, 0x6F03FA30 }
    __asm { jmp eax }
}
extern "C" __declspec(naked) int __fastcall Callee_6F285DC0(void*)
{
    __asm { mov eax, 0x6F285DC0 }
    __asm { jmp eax }
}
extern "C" __declspec(naked) void __fastcall Callee_6F39B810(void*, void*)
{
    __asm { mov eax, 0x6F39B810 }
    __asm { jmp eax }
}

}  // namespace

//----------------------------------------------------------------------------
//  0x6F420380 - log an "async set local leader" diagnostic (a no-op in this
//  NDEBUG build, NoOp_6F41F5E0) naming the unit's own owner/position, then
//  run the '+w3u' SCheckedUnitSlot::Assign type check against the unit and,
//  if it passes and the resulting agile-type record is otherwise untouched
//  (+0x20 == 0), notify the caller's own +0x3BC through 0x6F285DC0 before
//  submitting either the unit or null to 0x6F39B810.
//----------------------------------------------------------------------------
static const char aNull_0[] = "null";
static const char aAsyncSetLocalL[] =
    "Async set local leader to %s %x:%x for %s";

//  0x6F02A560 - SCheckedUnitSlot::Assign, already reconstructed for real
//  (Agent/unittypedassign.cpp).  Thunked here rather than called by its own
//  mangled member-function name: this call site's own `this` is a raw
//  `lea ecx,[edi+1E0h]` into a struct this file does not otherwise model,
//  not a real SCheckedUnitSlot object, so nothing here can spell the real
//  C++ call - see this file's own note on the whole family above for why
//  every callee outside this agent-6 slice's own worklist is thunked
//  rather than guessed at.
namespace {
extern "C" __declspec(naked) void* __fastcall Callee_6F02A560(void*, void*)
{
    __asm { mov eax, 0x6F02A560 }
    __asm { jmp eax }
}
}  // namespace

__declspec(naked) void __fastcall AsyncSetLocalLeaderNotifyAndSubmit(void*, void*)
{
    __asm
    {
        push    ebx
        push    ebp
        push    esi
        mov     esi, [esp+0Ch+4]
        test    esi, esi
        push    edi
        mov     edi, ecx
        jz      short noUnit
        mov     ecx, [esi+30h]
        mov     ebp, [esi+10h]
        mov     ebx, [esi+0Ch]
        xor     edx, edx
        call    Callee_6F32DBE0
        jmp     short haveOwner
noUnit:
        xor     ebp, ebp
        xor     ebx, ebx
        mov     eax, offset aNull_0
haveOwner:
        mov     ecx, [edi+1ACh]
        push    ecx
        push    ebp
        push    ebx
        push    eax
        push    offset aAsyncSetLocalL
        push    0
        call    NoOp_6F41F5E0
        add     esp, 18h
        push    esi
        lea     ecx, [edi+1E0h]
        call    Callee_6F02A560
        xor     edx, edx
        lea     ecx, [edx+1]
        call    Callee_6F300710
        test    esi, esi
        mov     edi, eax
        jz      short skip
        mov     edx, [esi+10h]
        mov     ecx, [esi+0Ch]
        call    Callee_6F03FA30
        test    eax, eax
        jz      short skip
        xor     edx, edx
        cmp     dword ptr [eax+0Ch], 2B61676Ch
        setnz   dl
        sub     edx, 1
        and     edx, eax
        mov     eax, edx
        jz      short skip
        cmp     dword ptr [eax+20h], 0
        jnz     short skip
        mov     ecx, esi
        call    Callee_6F285DC0
        test    eax, eax
        jz      short skip
        mov     ecx, [edi+3BCh]
        push    esi
        call    Callee_6F39B810
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        retn    4
skip:
        mov     ecx, [edi+3BCh]
        push    0
        call    Callee_6F39B810
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        retn    4
    }
}

namespace {

//  0x6F03FA30 already thunked above (Callee_6F03FA30); 0x6F49C690 is a
//  cunit_agent5_worklist.md worklist entry for a different concurrent
//  agent's slice, so it stays thunked here.
extern "C" __declspec(naked) void __fastcall Callee_6F49C690(void*)
{
    __asm { mov eax, 0x6F49C690 }
    __asm { jmp eax }
}

}  // namespace

//----------------------------------------------------------------------------
//  0x6F474CB0 - resolve this handle's owning player-side record (0x6F03FA30)
//  and forward its own +0xA8 field to the real per-record handler.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall ForwardField0A8ToHandler(void*, void*)
{
    __asm
    {
        mov     eax, [esp+4]
        mov     edx, [ecx+0Ch]
        mov     ecx, [ecx+8]
        push    eax
        call    Callee_6F03FA30
        mov     ecx, [eax+0A8h]
        call    Callee_6F49C690
        retn    4
    }
}

namespace {

//  0x6F49DBE0 - below this slice's own BFS cut; not claimed by any of the
//  eight concurrent slices.
extern "C" __declspec(naked) void Callee_6F49DBE0()
{
    __asm { mov eax, 0x6F49DBE0 }
    __asm { jmp eax }
}

}  // namespace

//----------------------------------------------------------------------------
//  0x6F49DFF0 - run a fixed four-argument setup call, then splice the low
//  byte of a stack argument into bits 31:30 of +0x88, leaving the low thirty
//  bits untouched.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall CUnit_SetField88TopTwoBits(void*, int)
{
    __asm
    {
        push    esi
        push    1
        push    0
        push    1
        push    0FFFFFFFFh
        mov     esi, ecx
        call    Callee_6F49DBE0
        movzx   eax, byte ptr [esp+4+4]
        mov     ecx, [esi+88h]
        shl     eax, 1Eh
        and     ecx, 3FFFFFFFh
        or      eax, ecx
        mov     [esi+88h], eax
        pop     esi
        retn    4
    }
}
