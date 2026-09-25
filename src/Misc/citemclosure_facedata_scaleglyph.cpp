//============================================================================
//  0x6F7CBBF0 - FACEDATA_ScaleGlyphSize.  CItem vtable slots 32/104
//  closure, batch K continuation (depth 12-25+).
//
//  Clamps a requested {width,height} pair to at least 1 each (defaulting
//  either that arrives 0 to the other), stores them as words into the
//  glyph record at `self->m_field58 + 0x0C`, and - only when
//  `self->m_field8` bit 0 is set - rescales both through
//  FACEDATA_FixedDiv against `self->m_field44` and refreshes the derived
//  metrics (FACEDATA_ComputeGlyphMetrics).  An optional callback off the
//  owner's own `+0x4C` (read through `self->m_field60->0x14`, the same
//  owner-descriptor chain FACEDATA_CreateGlyphEntry/InstallOwnerHookRecord
//  read) gets first refusal of the return value.  Returns 0x23 (and does
//  nothing else) when either `self->m_field58` or `self->m_field60` is
//  null.
//
//  `__declspec(naked)`: it calls FACEDATA_ComputeGlyphMetrics, whose own
//  `edi`/`esi` are real registers with no exposed calling convention (see
//  that file's own header, Misc/citemclosure_facedata_release.cpp) - `edi`
//  here is `self` itself and `esi` the record pointer, both already the
//  right values at that call site, so a naked body is the only way to
//  satisfy it without inventing a declaration that does not exist.  `ecx`
//  is `self`, `edx` the requested width, and the one stack argument
//  (`retn 4`) the requested height.
//============================================================================
#include "storm.h"

int __fastcall FACEDATA_FixedDiv(int a, int b);
void FACEDATA_ComputeGlyphMetrics();

__declspec(naked) void FACEDATA_ScaleGlyphSize()
{
    __asm {
        sub     esp, 8
        push    ebx
        push    esi
        push    edi
        mov     edi, ecx
        mov     eax, [edi + 0x58]
        test    eax, eax
        mov     ebx, edx
        mov     dword ptr [esp + 0x14 - 8], 0
        lea     esi, [eax + 0x0C]
        jz      fail

        mov     eax, [edi + 0x60]
        test    eax, eax
        jz      fail

        test    ebx, ebx
        mov     eax, [eax + 0x14]
        push    ebp
        mov     ebp, [esp + 0x18 + 4]
        mov     [esp + 0x18 - 4], eax
        jnz     edx_nonzero
        mov     ebx, ebp
        jmp     joined
edx_nonzero:
        test    ebp, ebp
        jnz     joined
        mov     ebp, ebx
joined:
        cmp     ebx, 1
        jnb     skip1
        mov     ebx, 1
skip1:
        cmp     ebp, 1
        jnb     skip2
        mov     ebp, 1
skip2:
        mov     [esi], bx
        mov     [esi + 2], bp
        test    byte ptr [edi + 8], 1
        jz      skip_fixed

        movzx   edx, word ptr [edi + 0x44]
        movzx   ecx, bx
        shl     ecx, 6
        call    FACEDATA_FixedDiv
        mov     [esi + 4], eax
        movzx   edx, word ptr [edi + 0x44]
        movzx   ecx, bp
        shl     ecx, 6
        call    FACEDATA_FixedDiv
        mov     [esi + 8], eax
        call    FACEDATA_ComputeGlyphMetrics
skip_fixed:
        mov     ecx, [esp + 0x18 - 4]
        mov     eax, [ecx + 0x4C]
        test    eax, eax
        jz      skip_callback

        mov     ecx, [edi + 0x58]
        push    ebp
        mov     edx, ebx
        call    eax
        mov     [esp + 0x18 - 8], eax
skip_callback:
        mov     eax, [esp + 0x18 - 8]
        pop     ebp
        pop     edi
        pop     esi
        pop     ebx
        add     esp, 8
        retn    4

fail:
        pop     edi
        pop     esi
        mov     eax, 0x23
        pop     ebx
        add     esp, 8
        retn    4
    }
}
