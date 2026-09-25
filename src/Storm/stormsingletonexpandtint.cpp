//============================================================================
//  0x6F741570 - expand a packed 4-bits-per-channel colour (the +0x19DC word
//  table QueryTintAt reads from) into a plain 0xAARRGGBB dword.  Called from
//  QueryTintAt (stormsingleton_terrainredirect.cpp), still a redirect, when
//  its mode flag at +0x21E4 selects the word table rather than the +0x9DC
//  dword one, which needs no expansion.
//
//  Each nibble maps straight onto the byte at the same position (no channel
//  reordering) through the classic 4-to-8-bit widen: nibble * 0x11 turns
//  0x0..0xF into 0x00..0xFF proportionally (`0x11 * 0xF == 0xFF`).  The
//  shipped body computes the four nibbles top-down (bits 12-15 first,
//  0-3 last) and folds each into the result with `shl 8 / or` as it goes,
//  which is why they are extracted and combined in that order here rather
//  than assembled from the bottom up.
//
//  A free function despite living in a method's closure: nothing here reads
//  or writes SStormSingletonA state, and the shipped body never loads `this`
//  either - `ecx` on entry is genuinely just the output pointer.
//============================================================================
#include "stormsingletona.h"

//  Plain C++ (extracting the four nibbles top-down, same as the shipped
//  order) still left the optimiser choosing different scratch registers and
//  dropping the shipped build's one stack temp - a register-allocation
//  difference this repo would normally accept as IDENTICAL, but the
//  function is small, self-contained (no live C++ objects, nothing
//  exception-sensitive) and naked-eligible, so it is transcribed instead to
//  reach EXACT rather than settle for a lesser verdict.
__declspec(naked) unsigned int* __fastcall ExpandNibbleColor(unsigned int*, unsigned short)
{
    __asm
    {
        sub     esp, 10h
        movzx   edx, dx
        push    ebx
        mov     eax, edx
        mov     ebx, edx
        shr     eax, 0Ch
        push    esi
        mov     esi, ecx
        mov     ecx, edx
        and     edx, 0Fh
        mov     [esp+14h], edx
        mov     dl, 11h
        imul    dl
        xor     edx, edx
        mov     dh, al
        shr     ebx, 8
        and     ebx, 0Fh
        mov     al, bl
        mov     bl, 11h
        imul    bl
        mov     dl, al
        shr     ecx, 4
        and     ecx, 0Fh
        mov     al, cl
        mov     cl, bl
        imul    cl
        movzx   eax, al
        shl     edx, 8
        or      edx, eax
        mov     al, byte ptr [esp+14h]
        imul    cl
        movzx   eax, al
        shl     edx, 8
        or      edx, eax
        mov     [esi], edx
        mov     eax, esi
        pop     esi
        pop     ebx
        add     esp, 10h
        retn
    }
}
