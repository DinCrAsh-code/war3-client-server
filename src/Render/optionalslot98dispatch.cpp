//============================================================================
//  Three small "if the +0x98 sub-object exists, do the real work; otherwise
//  return a default" dispatchers.  Each one's own dump includes the "real
//  work" chunk inline (a single-entry block IDA placed elsewhere in the
//  module and folded into the same listing, since nothing else reaches it)
//  so each is reproduced here as one complete naked body rather than a
//  guessed C++ shape plus a call to an unreconstructed helper.
//
//  None of the three has a caller in this batch's own dump, and two of the
//  three real-work chunks call further leaves this batch never reaches
//  (0x6F515180, 0x6F514A40) - declared as naked, unclaimed redirects (no
//  dump of their own; not part of any of the 8 CItem-closure batches) the
//  same way an out-of-scope callee is declared throughout this repo.
//============================================================================
#include "storm.h"

//  Forced out of intrinsic form (Sync/debuglock.cpp's own precedent): a
//  naked body's `call memset` needs a real link symbol, not the compiler
//  intrinsic /Oi would otherwise substitute, which is not a callable label.
extern "C" void* __cdecl memset(void* dst, int value, unsigned int count);
#pragma function(memset)

static const char s_canimobjBlenTag[] = ".?AUCAnimObjBlend@@";
static const char s_deleteArrayTag[] = "delete[]";

//----------------------------------------------------------------------------
//  0x6F514490 - the "real work" for sub_6F501CD0 below.  Not in this
//  batch's own dump (no `### sub_6F514490` heading anywhere in the combined
//  dump) and not part of any of the 8 CItem-closure batches; declared as a
//  naked, unclaimed redirect rather than guessed at.  `retn 4` inferred
//  from its caller's own null-path `retn 4`, which a tail jmp must match.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall Thunk_0x6F514490(int)
{
    __asm
    {
        mov     eax, 06F514490h
        jmp     eax
    }
}

//  0x6F501CD0 - `retn 4`.
__declspec(naked) void Thunk_0x6F501CD0()
{
    __asm
    {
        mov     ecx, [ecx+98h]
        test    ecx, ecx
        jz      short is_null
        jmp     Thunk_0x6F514490
    is_null:
        retn    4
    }
}

//----------------------------------------------------------------------------
//  0x6F501CF0 - same +0x98 guard, `retn 0` on the null path.  The real-work
//  chunk (originally at 0x6F512630) computes one `int` difference: given an
//  index in `edx`, `((+0x98)->+0x18)[edx].something - (+0x44->+0x18)[edx *
//  0x8C-stride].something`-shaped - reproduced literally rather than typed,
//  since neither operand's true element type is established by this call
//  tree.
//----------------------------------------------------------------------------
__declspec(naked) void Thunk_0x6F501CF0()
{
    __asm
    {
        mov     ecx, [ecx+98h]
        test    ecx, ecx
        jz      short is_null
        jmp     short real_work
    is_null:
        xor     eax, eax
        retn
    real_work:
        mov     eax, [ecx+44h]
        mov     ecx, [ecx+8]
        push    esi
        mov     esi, edx
        imul    esi, 08Ch
        push    edi
        mov     edi, [eax+18h]
        shl     edx, 4
        mov     eax, [ecx+edx]
        sub     eax, [edi+esi]
        pop     edi
        pop     esi
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F515180 / 0x6F514A40 - two leaves the real-work chunk below calls.
//  Neither has a dump of its own in this batch (no heading in the combined
//  dump) and neither is part of any of the 8 CItem-closure batches;
//  declared as naked, unclaimed redirects.  `retn 4` for both, confirmed
//  against build/*.asm by thunk_abi_audit.py - both are called with
//  exactly one stack dword pushed and neither caller adjusts esp
//  afterwards.  Declared `__stdcall` with that one parameter so the
//  declared arity agrees with the shipped callee's own cleanup - a naked
//  body ignores the calling convention keyword for codegen either way.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall Thunk_0x6F515180(int)
{
    __asm
    {
        mov     eax, 06F515180h
        jmp     eax
    }
}
__declspec(naked) void __stdcall Thunk_0x6F514A40(int)
{
    __asm
    {
        mov     eax, 06F514A40h
        jmp     eax
    }
}

//----------------------------------------------------------------------------
//  0x6F501D70 - same +0x98 guard, `retn 0` on both paths.  The real-work
//  chunk (originally at 0x6F515420) tests a flag byte at the sub-object's
//  own +0x54, and - only when the caller's `edx` is non-zero - sets bit
//  0x10 of it, resets two embedded blend-buffer sub-objects through the two
//  leaves above, `memset`s a buffer described by a {ptr@+0x00, count@+0x04}
//  pair the second leaf leaves at [edi]/[edi+4] (count doubled twice, i.e.
//  x4, before being handed to `memset` as a byte size), and returns 1.  The
//  `edx == 0` path instead clears bit 0x10, frees three Storm-tagged blocks
//  at +0x64/+0x6C/+0x70 if they were set, zeroes all three plus +0x68, and
//  also returns 1 - except when the +0x54 flag's sign bit was already
//  clear, which returns 0 without touching anything.
//----------------------------------------------------------------------------
__declspec(naked) void Thunk_0x6F501D70()
{
    __asm
    {
        mov     ecx, [ecx+98h]
        test    ecx, ecx
        jz      short is_null
        jmp     short real_work
    is_null:
        retn
    real_work:
        push    esi
        mov     esi, ecx
        mov     eax, [esi+54h]
        test    al, al
        js      short has_flag
        xor     eax, eax
        pop     esi
        retn
    has_flag:
        push    ebx
        xor     ebx, ebx
        cmp     edx, ebx
        jz      short clearing
        or      eax, 10h
        mov     [esi+54h], eax
        mov     eax, [esi+14h]
        push    edi
        push    eax
        lea     ecx, [esi+64h]
        call    Thunk_0x6F515180
        mov     ecx, [esi+14h]
        lea     edi, [esi+70h]
        push    ecx
        mov     ecx, edi
        call    Thunk_0x6F514A40
        mov     edx, [edi+4]
        mov     eax, [edi]
        add     edx, edx
        add     edx, edx
        push    edx
        push    ebx
        push    eax
        call    memset
        add     esp, 0Ch
        pop     edi
        pop     ebx
        mov     eax, 1
        pop     esi
        retn
    clearing:
        and     eax, 0FFFFFFEFh
        mov     [esi+54h], eax
        mov     eax, [esi+6Ch]
        cmp     eax, ebx
        jz      short skip_free1
        push    ebx
        push    0FFFFFFFEh
        push    offset s_canimobjBlenTag
        push    eax
        call    SMemFree
    skip_free1:
        mov     [esi+64h], ebx
        mov     [esi+68h], ebx
        mov     [esi+6Ch], ebx
        mov     eax, [esi+70h]
        cmp     eax, ebx
        jz      short skip_free2
        push    ebx
        push    0FFFFFFFFh
        push    offset s_deleteArrayTag
        push    eax
        call    SMemFree
    skip_free2:
        mov     [esi+70h], ebx
        mov     [esi+74h], ebx
        pop     ebx
        mov     eax, 1
        pop     esi
        retn
    }
}
