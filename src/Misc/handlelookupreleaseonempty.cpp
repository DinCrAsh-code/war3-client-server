//============================================================================
//  0x6F5018E0 - resolve one handle (`edx`'s own first dword, or 0 when
//  `edx` is null) through 0x6F500B50; if that lookup comes back null,
//  return 0 immediately.  Otherwise call 0x6F501700 (no arguments beyond
//  `this`), and only when *that* also comes back null, dispatch the
//  resolved handle's own vtable slot 4 (no arguments beyond `this`) before
//  returning 0x501700's own result.
//
//  Neither 0x6F500B50 nor 0x6F501700 is in this batch's own dump or in any
//  of the 8 CItem-closure batches; declared as naked, unclaimed redirects.
//  `retn 4` - one stack dword, the same one the dump's own `arg_0` names.
//============================================================================

//  0x6F500B50 - `this` (ecx) plus one stack dword; `retn 4` (confirmed
//  against build/*.asm by thunk_abi_audit.py).  Declared `__stdcall` with
//  that one parameter so the declared arity agrees with the shipped
//  callee's own cleanup - a naked body ignores the calling convention
//  keyword for codegen either way.
__declspec(naked) void __stdcall Thunk_0x6F500B50(int)
{
    __asm
    {
        mov     eax, 06F500B50h
        jmp     eax
    }
}

//  0x6F501700 - `this` (ecx) only, no stack arguments; `retn 0`.
__declspec(naked) void Thunk_0x6F501700()
{
    __asm
    {
        mov     eax, 06F501700h
        jmp     eax
    }
}

__declspec(naked) void* ResolveHandleReleaseIfEmpty(void* /*this*/, void* /*handle*/)
{
    __asm
    {
        push    esi
        mov     esi, edx
        test    esi, esi
        push    edi
        mov     eax, ecx
        jz      short use_zero
        mov     edx, [esi]
        jmp     short have_arg
    use_zero:
        xor     edx, edx
    have_arg:
        push    ebx
        mov     ebx, [esp+0Ch+8]
        push    edx
        call    Thunk_0x6F500B50
        mov     edi, eax
        test    edi, edi
        pop     ebx
        jnz     short have_result
        pop     edi
        xor     eax, eax
        pop     esi
        retn    4
    have_result:
        call    Thunk_0x6F501700
        mov     esi, eax
        test    esi, esi
        jnz     short skip_release
        mov     eax, [edi]
        mov     edx, [eax+4]
        mov     ecx, edi
        call    edx
    skip_release:
        pop     edi
        mov     eax, esi
        pop     esi
        retn    4
    }
}
