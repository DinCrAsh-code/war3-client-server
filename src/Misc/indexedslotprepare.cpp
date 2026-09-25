//============================================================================
//  0x6F4FAAB0 - resolve one slot (`this+8` when the stack argument is 0,
//  `*(this+4)+argument` otherwise), read the resolved slot's own +0x04
//  count and clamp it to >= 0 into `ebx`, call the unclaimed 0x6F4F8660
//  with the original stack argument, then dispatch vtable slot 0 of `this`
//  with the same argument (its own return value is discarded) and return
//  the clamped count computed earlier - not the vtable call's result.
//
//  0x6F4F8660 is not in this batch's own dump and is not part of any of
//  the 8 CItem-closure batches; declared as a naked, unclaimed redirect.
//  Both it and the vtable slot 0 dispatch are called `thiscall` with one
//  stack dword (the same `argument`), matching the two identical `push
//  edi` sites with no `add esp` after either call.  `retn 4`.
//============================================================================

//  0x6F4F8660 - see the file header.  `retn 4`, confirmed against
//  build/*.asm by thunk_abi_audit.py.  Declared `__stdcall` with one
//  parameter so the declared arity agrees with the shipped callee's own
//  cleanup - a naked body ignores the calling convention keyword for
//  codegen either way.
__declspec(naked) void __stdcall Thunk_0x6F4F8660(int)
{
    __asm
    {
        mov     eax, 06F4F8660h
        jmp     eax
    }
}

__declspec(naked) int PrepareIndexedSlot(void* /*this*/, int /*argument*/)
{
    __asm
    {
        push    ebx
        push    esi
        push    edi
        mov     edi, [esp+0Ch+8]
        test    edi, edi
        mov     esi, ecx
        jz      short use_default
        mov     eax, [esi+4]
        add     eax, edi
        jmp     short have_slot
    use_default:
        lea     eax, [esi+8]
    have_slot:
        mov     eax, [eax+4]
        xor     ebx, ebx
        test    eax, eax
        setle   bl
        push    edi
        sub     ebx, 1
        and     ebx, eax
        call    Thunk_0x6F4F8660
        mov     eax, [esi]
        mov     edx, [eax]
        push    edi
        mov     ecx, esi
        call    edx
        pop     edi
        pop     esi
        mov     eax, ebx
        pop     ebx
        retn    4
    }
}
