//============================================================================
//  0x6F4E8200 - resolve one element of an `eax`-held array-of-pointers by
//  `edx` index, zero the two dwords at that element's +0x10/+0x14 (after
//  storing 0.0f at +0x18 first), then tail-jump into vtable slot 0x0C of
//  that same element.  No stack arguments and no `retn` of its own - the
//  tail jump supplies whatever cleanup the resolved slot itself does.
//
//  Neither register holds a value this call tree can name (`eax` arrives
//  already pointing at an array-of-pointers holder with the array itself at
//  +0x10, `edx` an index): no caller is in this batch's own dump, so this
//  is reproduced as a literal instruction-for-instruction naked body rather
//  than a guessed C++ signature.
//============================================================================
__declspec(naked) void Thunk_0x6F4E8200()
{
    __asm
    {
        mov     ecx, [eax+10h]
        fldz
        mov     ecx, [ecx+edx*4]
        fstp    dword ptr [ecx+18h]
        xor     eax, eax
        mov     [ecx+10h], eax
        mov     [ecx+14h], eax
        mov     eax, [ecx]
        mov     eax, [eax+0Ch]
        jmp     eax
    }
}
