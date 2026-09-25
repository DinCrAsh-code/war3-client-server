//============================================================================
//  0x6F4EC9F0 - append one zeroed 0x1C-byte record to a growable array
//  (capacity @ +0x00, count @ +0x04, data @ +0x08, a "growth chunk" divisor
//  @ +0x0C), growing first through 0x6F4EB210 when the array is already at
//  capacity - rounding the requested new count up to a multiple of the
//  chunk size first - then committing the grow through 0x6F4EC320.  Returns
//  a pointer to the freshly zeroed record, or null if the count overflowed
//  to zero mid-grow (the `test eax,eax` guards a null data pointer, not a
//  failed grow specifically).
//
//  Neither 0x6F4EB210 nor 0x6F4EC320 is in this batch's own dump or in any
//  of the 8 CItem-closure batches; declared as naked, unclaimed redirects.
//  `retn 4` each, confirmed against build/*.asm by thunk_abi_audit.py (one
//  stack dword pushed at both call sites here, callee-cleaned) - declared
//  `__stdcall` with that one parameter so the declared arity agrees with
//  the shipped callee's own cleanup, even though a naked body ignores the
//  calling convention keyword for codegen; only the caller-visible cleanup
//  byte count this audit reads out of the declaration is at stake.
//============================================================================

//  0x6F4EB210 - grow the array's capacity for at least `newCount` records,
//  returning the new capacity (divided into `edx:eax` by the caller's own
//  `div`, so it is read back through `eax`).
__declspec(naked) void __stdcall Thunk_0x6F4EB210(unsigned int /*newCount*/)
{
    __asm
    {
        mov     eax, 06F4EB210h
        jmp     eax
    }
}

//  0x6F4EC320 - commit the array to exactly `newCount` records (the
//  rounded-up value when a grow just happened, the untouched requested
//  count otherwise).
__declspec(naked) void __stdcall Thunk_0x6F4EC320(unsigned int /*newCount*/)
{
    __asm
    {
        mov     eax, 06F4EC320h
        jmp     eax
    }
}

__declspec(naked) void* AppendGrowRecord()
{
    __asm
    {
        push    esi
        push    edi
        mov     edi, ecx
        mov     esi, [edi+4]
        add     esi, 1
        cmp     esi, [edi]
        jbe     short after_grow
        mov     ecx, [edi+0Ch]
        test    ecx, ecx
        jnz     short have_chunk
        push    esi
        mov     ecx, edi
        call    Thunk_0x6F4EB210
        mov     ecx, eax
    have_chunk:
        xor     edx, edx
        mov     eax, esi
        div     ecx
        test    edx, edx
        jz      short no_remainder
        sub     ecx, edx
        add     ecx, esi
        mov     esi, ecx
    no_remainder:
        push    esi
        mov     ecx, edi
        call    Thunk_0x6F4EC320
    after_grow:
        mov     ecx, [edi+4]
        mov     edx, [edi+8]
        lea     eax, ds:0[ecx*8]
        sub     eax, ecx
        add     ecx, 1
        lea     eax, [edx+eax*4]
        test    eax, eax
        mov     [edi+4], ecx
        pop     edi
        pop     esi
        jz      short zero_case
        mov     dword ptr [eax], 0
        mov     dword ptr [eax+4], 0
        mov     dword ptr [eax+8], 0
        mov     dword ptr [eax+0Ch], 0
        retn
    zero_case:
        xor     eax, eax
        retn
    }
}
