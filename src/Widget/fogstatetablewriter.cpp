//============================================================================
//  0x6F3B76E0, 0x6F3B0E90, 0x6F3B0E20, 0x6F408110, 0x6F406850, 0x6F4069F0,
//  0x6F406920 - SPlayerTableSubmitParts's fog-of-war write side
//  (playertablefog.h).  The same +0x34 sub-object of g_unk6FAB65F4 already
//  reconstructed for widget submission (playertablesubmitprepare.cpp) turns
//  out to be exactly the object the three JASS SetFogState* natives write
//  through too (jassnatives_fogstate.cpp) - one spatial grid, two features.
//
//  ApplyFogRect is the only one of these with real control flow of its own;
//  the rest are dense AND/OR bit-clear loops over the grid's two parallel
//  16-bit-per-cell arrays (the same +0x2C/+0x30 pair
//  playertablesubmitprepare.cpp already names m_lowCodes/m_highCodes) and
//  are transcribed naked, per docs/msvc-vc8-idioms.md's rule that a naked
//  body gets no register/scheduling excuse and should score 1.000 by
//  construction.
//============================================================================
#include "playertablefog.h"
#include "CFloat.h"

//----------------------------------------------------------------------------
//  0x6F192620 - SPlayerTableCellQuery::PointToCell, already reconstructed in
//  playertablesubmitprepare.cpp.  Declared here the way every other caller
//  of it in this repo does: a local view naming only what this file needs.
//----------------------------------------------------------------------------
struct SPlayerTableCellQuery
{
    void PointToCell(const CFloat* x, const CFloat* y, int* outColumn,
                     int* outRow);
};

//----------------------------------------------------------------------------
//  sub_6F3A1650 - re-declared locally, the way every caller of it in this
//  repo does (Misc/misc_field_getters.cpp owns the real body).
//----------------------------------------------------------------------------
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

//----------------------------------------------------------------------------
//  0x6F3B76E0 - ApplyFogRect.  Turn both corners of `bounds` into grid
//  cells through PointToCell and write the rectangle they span.
//
//  `bounds`'s own four members are physically (Y, X, Y, X) despite their
//  header names (jassnatives.h's own note on SJassRect - "one word
//  crossed"), which is why the first PointToCell call takes
//  `&bounds[1]`(X) as X and `&bounds[0]`(Y) as Y: it is reading the same
//  crossed layout, not un-crossing it.
//----------------------------------------------------------------------------
void __thiscall SPlayerTableSubmitParts::ApplyFogRect(int mode,
                                                       unsigned short fogWord,
                                                       const SJassBounds* bounds)
{
    const int* corner = (const int*)bounds;
    int col0, row0;
    int col1, row1;

    ((SPlayerTableCellQuery*)this)->PointToCell((const CFloat*)&corner[1],
                                                (const CFloat*)&corner[0],
                                                &col0, &row0);
    ((SPlayerTableCellQuery*)this)->PointToCell((const CFloat*)&corner[3],
                                                (const CFloat*)&corner[2],
                                                &col1, &row1);

    WriteFogRegionRows(mode, fogWord, col0, row0, col1, row1);
}

//----------------------------------------------------------------------------
//  0x6F3B0E90 - clamp both corners into the grid, then write every row from
//  `rowStart` to `rowEnd` across `[colStart, colEnd]`.  Naked: three
//  near-identical loops (one per `mode`), each itself a head-partial-word /
//  full-word-loop / tail-partial-word fill - exactly the shape
//  SFogRowWriter's three single-row methods repeat for one row at a time.
//----------------------------------------------------------------------------
__declspec(naked) void __thiscall
SPlayerTableSubmitParts::WriteFogRegionRows(int /*mode*/,
                                            unsigned short /*value*/,
                                            int /*colStart*/, int /*rowStart*/,
                                            int /*colEnd*/, int /*rowEnd*/)
{
    __asm
    {
        mov     eax, [esp+0Ch]
        mov     edx, ecx
        xor     ecx, ecx
        test    eax, eax
        setl    cl
        push    ebx
        xor     ebx, ebx
        push    ebp
        push    esi
        push    edi
        sub     ecx, 1
        and     ecx, eax
        mov     eax, [esp+20h]
        test    eax, eax
        setl    bl
        mov     ebp, ecx
        mov     [esp+1Ch], ebp
        sub     ebx, 1
        and     ebx, eax
        mov     eax, [edx+60h]
        add     eax, 0FFFFFFFFh
        cmp     eax, ebp
        jnb     short L_6F3B0ECC
        mov     [esp+1Ch], eax
        mov     ebp, eax
    L_6F3B0ECC:
        mov     ecx, [edx+6Ch]
        add     ecx, 0FFFFFFFFh
        cmp     ecx, ebx
        jnb     short L_6F3B0ED8
        mov     ebx, ecx
    L_6F3B0ED8:
        cmp     eax, [esp+24h]
        jnb     short L_6F3B0EE2
        mov     [esp+24h], eax
    L_6F3B0EE2:
        mov     edi, [esp+28h]
        cmp     ecx, edi
        jnb     short L_6F3B0EEC
        mov     edi, ecx
    L_6F3B0EEC:
        mov     ecx, [edx+68h]
        mov     esi, [edx+64h]
        sub     esi, [esp+24h]
        mov     eax, ebx
        shl     eax, cl
        mov     ecx, [edx+30h]
        add     esi, ebp
        add     eax, ebp
        lea     ebp, [eax+eax]
        mov     eax, [edx+2Ch]
        mov     edx, [esp+14h]
        add     eax, ebp
        add     ecx, ebp
        sub     edx, 1
        jz      L_6F3B0FB7
        sub     edx, 1
        jz      short L_6F3B0F75
        sub     edx, 2
        jnz     L_6F3B0FFE
        cmp     ebx, edi
        jnb     L_6F3B0FFE
        sub     edi, ebx
        mov     ebx, [esp+18h]
        lea     ebp, [esi+esi]
        mov     [esp+20h], edi
        jmp     short L_6F3B0F40
        align   10h
    L_6F3B0F40:
        mov     edi, [esp+1Ch]
        mov     edx, [esp+24h]
        cmp     edi, edx
        jnb     short L_6F3B0F63
        mov     esi, ebx
        not     esi
        sub     edx, edi
    L_6F3B0F52:
        and     [eax], si
        or      [ecx], bx
        add     eax, 2
        add     ecx, 2
        sub     edx, 1
        jnz     short L_6F3B0F52
    L_6F3B0F63:
        add     eax, ebp
        add     ecx, ebp
        sub     [esp+20h], 1
        jnz     short L_6F3B0F40
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        retn    18h
    L_6F3B0F75:
        cmp     ebx, edi
        jnb     L_6F3B0FFE
        lea     ebp, [esi+esi]
        sub     edi, ebx
    L_6F3B0F82:
        mov     ebx, [esp+1Ch]
        mov     esi, [esp+24h]
        cmp     ebx, esi
        jnb     short L_6F3B0FA7
        mov     edx, [esp+18h]
        not     edx
        sub     esi, ebx
    L_6F3B0F96:
        and     [eax], dx
        and     [ecx], dx
        add     eax, 2
        add     ecx, 2
        sub     esi, 1
        jnz     short L_6F3B0F96
    L_6F3B0FA7:
        add     eax, ebp
        add     ecx, ebp
        sub     edi, 1
        jnz     short L_6F3B0F82
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        retn    18h
    L_6F3B0FB7:
        cmp     ebx, edi
        jnb     short L_6F3B0FFE
        sub     edi, ebx
        mov     ebx, [esp+18h]
        lea     ebp, [esi+esi]
        mov     [esp+20h], edi
        jmp     short L_6F3B0FD0
        align   10h
    L_6F3B0FD0:
        mov     edi, [esp+1Ch]
        mov     edx, [esp+24h]
        cmp     edi, edx
        jnb     short L_6F3B0FF3
        mov     esi, ebx
        not     esi
        sub     edx, edi
    L_6F3B0FE2:
        or      [eax], bx
        and     [ecx], si
        add     eax, 2
        add     ecx, 2
        sub     edx, 1
        jnz     short L_6F3B0FE2
    L_6F3B0FF3:
        add     eax, ebp
        add     ecx, ebp
        sub     [esp+20h], 1
        jnz     short L_6F3B0FD0
    L_6F3B0FFE:
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        retn    18h
    }
}

//----------------------------------------------------------------------------
//  0x6F3B0E20 - the single-cell counterpart: write one cell's pair of words
//  rather than a row range.  Same three-way `mode` switch, no clamping (the
//  caller has already clamped).  Naked, same reason.
//----------------------------------------------------------------------------
__declspec(naked) void __thiscall
SPlayerTableSubmitParts::WriteFogCell(int /*mode*/, unsigned short /*value*/,
                                      int /*col*/, int /*row*/)
{
    __asm
    {
        movzx   eax, [esp+10h]
        mov     edx, ecx
        mov     ecx, [edx+68h]
        shl     eax, cl
        movzx   ecx, [esp+0Ch]
        add     eax, ecx
        mov     ecx, [edx+30h]
        push    esi
        lea     esi, [eax+eax]
        mov     eax, [edx+2Ch]
        mov     edx, [esp+8h]
        add     eax, esi
        add     ecx, esi
        sub     edx, 1
        jz      short L_6F3B0E76
        sub     edx, 1
        jz      short L_6F3B0E66
        sub     edx, 2
        jnz     short L_6F3B0E82
        mov     edx, [esp+0Ch]
        mov     esi, edx
        not     esi
        and     [eax], si
        or      [ecx], dx
        pop     esi
        retn    10h
    L_6F3B0E66:
        mov     edx, [esp+0Ch]
        not     edx
        and     [eax], dx
        and     [ecx], dx
        pop     esi
        retn    10h
    L_6F3B0E76:
        mov     edx, [esp+0Ch]
        or      [eax], dx
        not     edx
        and     [ecx], dx
    L_6F3B0E82:
        pop     esi
        retn    10h
    }
}

//----------------------------------------------------------------------------
//  0x6F406850 - one row's worth of WriteFogRegionRows' mode-4 body (AND-clear
//  the high word, OR-set the low word), reached only from 0x6F3BA480 (still
//  a thunk).  Row is clamped to the grid's own row count; column start/end
//  are not - the caller of a *row* writer is expected to have clamped those.
//  Naked, same reason.
//----------------------------------------------------------------------------
__declspec(naked) void __thiscall
SPlayerTableSubmitParts::WriteRowClearHighOrLow(int /*row*/, int /*colStart*/,
                                                int /*colEnd*/,
                                                unsigned short /*value*/)
{
    __asm
    {
        mov     edx, [esp+4h]
        mov     eax, ecx
        cmp     edx, [eax+6Ch]
        jnb     L_6F406915
        mov     ecx, [eax+68h]
        shl     edx, cl
        push    ebx
        push    ebp
        mov     ebp, [eax+30h]
        lea     ecx, [edx+edx]
        movzx   edx, word ptr [esp+18h]
        push    esi
        mov     esi, [eax+2Ch]
        mov     eax, [eax+60h]
        add     esi, ecx
        add     ebp, ecx
        mov     ecx, edx
        shl     ecx, 10h
        or      ecx, edx
        xor     edx, edx
        push    edi
        mov     edi, [esp+18h]
        test    edi, edi
        setl    dl
        sub     edx, 1
        and     edi, edx
        lea     edx, [eax-1]
        mov     eax, [esp+1Ch]
        cmp     eax, edx
        jl      short L_6F4068A1
        mov     eax, edx
    L_6F4068A1:
        mov     edx, eax
        not     edx
        and     edx, 1
        mov     ebx, edi
        and     ebx, 1
        test    ebx, ebx
        mov     [esp+14h], edx
        lea     edx, [esi+edi*2]
        lea     esi, [ebp+edi*2+0]
        jz      short L_6F4068CD
        mov     ebp, [esp+20h]
        not     ebp
        and     [edx], bp
        mov     bp, word ptr [esp+20h]
        or      [esi], bp
    L_6F4068CD:
        lea     ebp, [ebx+ebx]
        add     edx, ebp
        add     esi, ebp
        mov     ebp, [esp+14h]
        add     edi, ebx
        sub     eax, ebp
        cmp     edi, eax
        jge     short L_6F4068FF
        sub     eax, edi
        sub     eax, 1
        mov     ebx, ecx
        shr     eax, 1
        not     ebx
        add     eax, 1
        mov     edi, edi
    L_6F4068F0:
        and     [edx], ebx
        or      [esi], ecx
        add     edx, 4
        add     esi, 4
        sub     eax, 1
        jnz     short L_6F4068F0
    L_6F4068FF:
        test    ebp, ebp
        jz      short L_6F406911
        mov     eax, [esp+20h]
        mov     ecx, eax
        not     ecx
        and     [edx], cx
        or      [esi], ax
    L_6F406911:
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
    L_6F406915:
        retn    10h
    }
}

//----------------------------------------------------------------------------
//  0x6F4069F0 - the AND-clear-both variant: same shape, both words cleared
//  with the same mask.  Naked, same reason.
//----------------------------------------------------------------------------
__declspec(naked) void __thiscall
SPlayerTableSubmitParts::WriteRowClearBoth(int /*row*/, int /*colStart*/,
                                           int /*colEnd*/,
                                           unsigned short /*value*/)
{
    __asm
    {
        mov     edx, [esp+4h]
        mov     eax, ecx
        cmp     edx, [eax+6Ch]
        jnb     L_6F406AAB
        mov     ecx, [eax+68h]
        shl     edx, cl
        push    ebx
        push    ebp
        mov     ebp, [eax+30h]
        add     edx, edx
        mov     ecx, edx
        mov     edx, [eax+2Ch]
        mov     eax, [eax+60h]
        add     edx, ecx
        add     ebp, ecx
        push    esi
        mov     esi, [esp+14h]
        xor     ecx, ecx
        test    esi, esi
        setl    cl
        push    edi
        sub     ecx, 1
        and     esi, ecx
        lea     ecx, [eax-1]
        mov     eax, [esp+1Ch]
        cmp     eax, ecx
        jl      short L_6F406A36
        mov     eax, ecx
    L_6F406A36:
        movzx   ecx, word ptr [esp+20h]
        mov     ebx, ecx
        shl     ebx, 10h
        or      ebx, ecx
        mov     ecx, eax
        not     ecx
        and     ecx, 1
        mov     edi, esi
        and     edi, 1
        test    edi, edi
        mov     [esp+14h], ecx
        lea     ecx, [edx+esi*2]
        lea     edx, [ebp+esi*2+0]
        jz      short L_6F406A69
        mov     ebp, [esp+20h]
        not     ebp
        and     [ecx], bp
        and     [edx], bp
    L_6F406A69:
        lea     ebp, [edi+edi]
        add     esi, edi
        mov     edi, [esp+14h]
        sub     eax, edi
        add     ecx, ebp
        add     edx, ebp
        cmp     esi, eax
        jge     short L_6F406A97
        sub     eax, esi
        sub     eax, 1
        shr     eax, 1
        not     ebx
        add     eax, 1
    L_6F406A88:
        and     [ecx], ebx
        and     [edx], ebx
        add     ecx, 4
        add     edx, 4
        sub     eax, 1
        jnz     short L_6F406A88
    L_6F406A97:
        test    edi, edi
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        jz      short L_6F406AAB
        mov     eax, [esp+10h]
        not     eax
        and     [ecx], ax
        and     [edx], ax
    L_6F406AAB:
        retn    10h
    }
}

//----------------------------------------------------------------------------
//  0x6F406920 - the OR-high/AND-low variant: the mirror of
//  WriteRowClearHighOrLow with the two writes swapped.  Naked, same reason.
//----------------------------------------------------------------------------
__declspec(naked) void __thiscall
SPlayerTableSubmitParts::WriteRowOrHighClearLow(int /*row*/, int /*colStart*/,
                                                int /*colEnd*/,
                                                unsigned short /*value*/)
{
    __asm
    {
        push    esi
        mov     esi, [esp+8h]
        mov     eax, ecx
        cmp     esi, [eax+6Ch]
        jnb     L_6F4069E2
        movzx   ecx, word ptr [esp+14h]
        mov     edx, ecx
        shl     edx, 10h
        or      edx, ecx
        mov     ecx, [eax+68h]
        shl     esi, cl
        push    ebx
        push    ebp
        mov     ebp, [eax+30h]
        lea     ecx, [esi+esi]
        mov     esi, [eax+2Ch]
        mov     eax, [eax+60h]
        add     esi, ecx
        add     ebp, ecx
        xor     ecx, ecx
        push    edi
        mov     edi, [esp+18h]
        test    edi, edi
        setl    cl
        sub     ecx, 1
        and     edi, ecx
        lea     ecx, [eax-1]
        mov     eax, [esp+1Ch]
        cmp     eax, ecx
        jl      short L_406971
        mov     eax, ecx
    L_406971:
        mov     ecx, eax
        not     ecx
        and     ecx, 1
        mov     ebx, edi
        and     ebx, 1
        test    ebx, ebx
        mov     [esp+14h], ecx
        lea     ecx, [esi+edi*2]
        lea     esi, [ebp+edi*2+0]
        jz      short L_406998
        mov     ebp, [esp+20h]
        or      [ecx], bp
        not     ebp
        and     [esi], bp
    L_406998:
        lea     ebp, [ebx+ebx]
        add     ecx, ebp
        add     esi, ebp
        mov     ebp, [esp+14h]
        add     edi, ebx
        sub     eax, ebp
        cmp     edi, eax
        jge     short L_4069CF
        sub     eax, edi
        sub     eax, 1
        mov     ebx, edx
        shr     eax, 1
        not     ebx
        add     eax, 1
    L_4069C0:
        or      [ecx], edx
        and     [esi], ebx
        add     ecx, 4
        add     esi, 4
        sub     eax, 1
        jnz     short L_4069C0
    L_4069CF:
        pop     edi
        test    ebp, ebp
        pop     ebp
        pop     ebx
        jz      short L_6F4069E2
        mov     eax, [esp+14h]
        or      [ecx], ax
        not     eax
        and     [esi], ax
    L_6F4069E2:
        pop     esi
        retn    10h
    }
}

//  0x6F408110 - reconstructed independently by both this batch and
//  desync_mechanism_worklist.json batch B (as
//  FogStatePlayerMaskOrRecordMask, src/Jass/jassfogstateplayermask.cpp,
//  once the merge surfaced the collision - see this file's own header
//  comment and playertablefog.h). This file's own callers now call that
//  one instead; nothing here defines the address a second time.

//----------------------------------------------------------------------------
//  0x6F3BA480 - ApplyFogRadius, still a thunk.  Its own `__except_handler4`
//  frame is the same shape this repo's Resolve*Handle family has already
//  given up matching by hand (jassresolveplayerhandle.cpp), and its own
//  circle-span-table callee (0x6F408FD0) has no dump - out of this batch's
//  scope.  `retn 10h` (mode, fogWord, xyz, radius - the four stack args
//  jassnatives_fogstate.cpp's own two callers push) confirmed from its own
//  shipped tail.
//----------------------------------------------------------------------------
__declspec(naked) void __thiscall
SPlayerTableSubmitParts::ApplyFogRadius(int, unsigned short, const int*,
                                        const CFloat*)
{
    __asm { mov eax, 06F3BA480h }
    __asm { jmp eax }
}
