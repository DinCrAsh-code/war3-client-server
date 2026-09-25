//============================================================================
//  0x6F4B7490 - fill a clamped [start, start+count) range of a fixed-size
//  record array with repeated copies of one 0x40-byte source record.
//
//  `this` (ecx) is the array object: a data pointer at +0x0C and an
//  allocated-slot count at +0x1C.  Its only caller in this call tree is
//  Storm/textsinkscratch.cpp's own SScratchRingStorage::Init (0x6F4B7560,
//  itself still a naked redirect, no dump of its own in this batch), which
//  is exactly the "32-entry, 0x40-byte-slot scratch ring buffer" that file's
//  header comment already describes - so this is very likely
//  SScratchRingStorage's own array-fill helper, called once from Init with
//  a zeroed template record to initialise every slot.  Nothing here asserts
//  more of that class's layout than the two fields this body itself reads,
//  and it is kept in a class of its own (SScratchRingArray) rather than
//  folded into SScratchRingStorage: Init's real body is not in this batch,
//  so there is no confirmed call site connecting the two beyond the address
//  neighbourhood and the shape match.
//
//  Three stack dwords - start index, source record pointer, requested copy
//  count - `retn 0Ch`.  Left a literal instruction-for-instruction naked
//  translation: the clamp (`min(start+count, alloc)`) and the broadcast copy
//  (`rep movsd` re-reading the *same* source pointer every iteration, so
//  every filled slot gets an identical copy rather than a range copy) are
//  exactly what the dump shows and not worth reinventing a guessed C++
//  shape for.
//============================================================================
struct SScratchRingArray
{
    void __thiscall FillRange(unsigned int start, const void* src, unsigned int count);
};

__declspec(naked) void __thiscall SScratchRingArray::FillRange(unsigned int, const void*,
                                                                unsigned int)
{
    __asm
    {
        mov     eax, [esp+0Ch]
        push    esi
        mov     esi, [esp+8]
        mov     edx, ecx
        mov     ecx, [edx+1Ch]
        add     eax, esi
        cmp     eax, ecx
        jnb     short skip_clamp
        mov     ecx, eax
    skip_clamp:
        cmp     esi, ecx
        jnb     short done
        push    ebx
        mov     eax, esi
        push    ebp
        mov     ebp, [esp+0Ch+8]
        shl     eax, 6
        sub     ecx, esi
        push    edi
        mov     ebx, ecx
        lea     ebx, [ebx+0]
    loop_top:
        mov     edi, [edx+0Ch]
        add     edi, eax
        mov     ecx, 10h
        mov     esi, ebp
        add     eax, 40h
        sub     ebx, 1
        rep movsd
        jnz     short loop_top
        pop     edi
        pop     ebp
        pop     ebx
    done:
        pop     esi
        retn    0Ch
    }
}
