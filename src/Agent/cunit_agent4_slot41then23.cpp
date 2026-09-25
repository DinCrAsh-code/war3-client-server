//============================================================================
//  0x6F021250 - a thiscall forwarder that calls one virtual slot for effect
//  and tail-jumps into another with the same receiver: `this`'s own vtbl
//  slot 0x29 (+0xA4) is called with no other argument and its result
//  discarded, then slot 0x17 (+0x5C) is tail-jumped into with the same
//  `this` in ecx, so whatever it returns (and however many bytes its own
//  `retn` cleans) becomes this function's own.
//
//  Written `__declspec(naked)`: a plain C++ two-virtual-call spelling would
//  have to invent argument counts and a return type for both slots to
//  express the call/tail-call as source, and neither slot has a
//  reconstruction in this session's own worklist to confirm either against
//  (0x6F267140/0x6F28B070 sit at CUnit's own +0xA4/+0x5C in
//  docs/targets/vtables/CUnit.txt, but the zero stack arguments this
//  function actually pushes do not agree with 0x6F267140's own `retn 4`
//  reading one, which means `this` here is not necessarily a CUnit at
//  all - a receiver ambiguity tools/vtable_dispatch_audit.py's own rule
//  treats as unknown rather than guessable). A naked transcription sidesteps
//  the guess entirely: every register and every instruction is the one the
//  dump names, so it is exact by construction rather than by argument.
//============================================================================

__declspec(naked) void CallSlot0x29ThenTailSlot0x17(void* /*thisPtr*/)
{
    __asm
    {
        push    esi
        mov     esi, ecx
        mov     eax, [esi]
        mov     edx, [eax+0A4h]
        call    edx
        mov     eax, [esi]
        mov     edx, [eax+5Ch]
        mov     ecx, esi
        pop     esi
        jmp     edx
    }
}
