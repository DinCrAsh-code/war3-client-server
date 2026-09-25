//============================================================================
//  0x6F7C9A30 - FACEDATA_AcquireOwnerBinding.  CItem vtable slots 32/104
//  closure, batch K continuation (depth 12-25+).
//
//  Allocates a binding record (sized from `self->m_owner->m_desc->m_size`,
//  reached the same way FACEDATA_CreateGlyphEntry's own does,
//  Misc/citemclosure_facedata_createglyph.cpp), acquires the owner hook on
//  it (FACEDATA_AcquireOwnerHook), and - purely as a self-check - releases
//  it straight back through FACEDATA_ReleaseOwnerHook and
//  FACEDATA_ReleaseIfSet before returning success; a failed hook
//  acquisition instead hands the half-built record back through `*outPtr`
//  for the caller to deal with.  `__declspec(naked)` because it calls both
//  owner-hook helpers, whose own `edi`/`esi` parameters are real registers
//  they read at entry with no exposed calling convention (see that file's
//  own header) - the same reason FACEDATA_LoadFaceEntry is naked.  Three
//  early-outs (a null `self`, a null `outPtr`, a null `self->m_owner`)
//  answer 6 without allocating anything.
//============================================================================
#include "storm.h"

int __fastcall FACEDATA_AllocateAndZero(void* self, int size, void** outPtr);
void __fastcall FACEDATA_ReleaseIfSet(void* self, void** slot);
void FACEDATA_AcquireOwnerHook();
void FACEDATA_ReleaseOwnerHook();

__declspec(naked) void FACEDATA_AcquireOwnerBinding()
{
    __asm {
        sub     esp, 8
        push    esi
        mov     esi, ecx
        test    esi, esi
        mov     [esp + 0x0C - 4], edx
        jz      fail
        test    edx, edx
        jz      fail
        cmp     dword ptr [esi + 0x60], 0
        jz      fail

        mov     dword ptr [edx], 0
        mov     eax, [esi + 0x60]
        mov     ecx, [eax + 0x14]
        mov     edx, [ecx + 0x2C]
        push    ebx
        push    ebp
        mov     ebp, [eax + 8]
        lea     eax, [esp + 0x14 - 8]
        push    eax
        mov     ecx, ebp
        call    FACEDATA_AllocateAndZero
        mov     ebx, eax
        test    ebx, ebx
        jnz     have_result

        push    edi
        mov     edi, [esp + 0x18 - 8]
        mov     [edi + 4], esi
        call    FACEDATA_AcquireOwnerHook
        mov     ebx, eax
        test    ebx, ebx
        jz      success_path

        mov     ecx, [esp + 0x18 - 4]
        mov     [ecx], edi
        pop     edi
        jmp     have_result

success_path:
        mov     esi, edi
        call    FACEDATA_ReleaseOwnerHook
        lea     edx, [esp + 0x18 - 8]
        mov     ecx, ebp
        call    FACEDATA_ReleaseIfSet
        pop     edi
        mov     eax, ebx
        pop     ebp
        pop     ebx
        pop     esi
        add     esp, 8
        retn

have_result:
        pop     ebp
        mov     eax, ebx
        pop     ebx
        pop     esi
        add     esp, 8
        retn

fail:
        mov     eax, 6
        pop     esi
        add     esp, 8
        retn
    }
}
