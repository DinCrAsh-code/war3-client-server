//============================================================================
//  0x6F48D220 - NIpse::CPoLink::ComputeChecksum, vtable slot 15.
//  CRlProperty::ComputeChecksum(hash) via the base, then folds
//  m_targetHandle (+0x78) then m_targetSub (+0x7C) into `*hash` via the
//  same per-byte `(b*0x7A2Dh) ^ b` mix Agent/cpoflag_checksum.cpp's own
//  single-field version already documents, chained across two fields (the
//  running `*hash` value carried forward in a register between them rather
//  than reloaded a second time). m_field80 is NOT folded here - only
//  Save/Load ever touch it (see cpolink.h's own file header).
//
//  Written naked for the same reason as every other member of this
//  family's own ComputeChecksum override - see
//  Agent/cpoflag_checksum.cpp's own header for the investigation this does
//  not re-litigate.
//============================================================================
#include "cpolink.h"
#include "rlproperty.h"

namespace NIpse {

__declspec(naked) void CPoLink::ComputeChecksum(unsigned int* /*hash*/)
{
    __asm
    {
        push    ebx
        push    esi
        mov     esi, [esp+12]
        push    edi
        push    esi
        mov     edi, ecx
        call    CRlProperty::ComputeChecksum
        mov     ecx, [edi+78h]
        mov     eax, ecx
        shr     eax, 10h
        movzx   edx, ah
        mov     ebx, edx
        imul    ebx, 7A2Dh
        xor     ebx, edx
        add     [esi], ebx
        mov     edx, [esi]
        movzx   eax, al
        mov     ebx, eax
        imul    ebx, 7A2Dh
        xor     ebx, eax
        lea     eax, [ebx+edx]
        movzx   edx, ch
        mov     ebx, edx
        imul    ebx, 7A2Dh
        xor     ebx, edx
        movzx   ecx, cl
        mov     edx, ecx
        imul    edx, 7A2Dh
        add     eax, ebx
        xor     edx, ecx
        add     edx, eax
        mov     [esi], edx
        mov     ecx, [edi+7Ch]
        mov     eax, ecx
        shr     eax, 10h
        movzx   edi, ah
        mov     ebx, edi
        imul    ebx, 7A2Dh
        xor     ebx, edi
        movzx   eax, al
        mov     edi, eax
        imul    edi, 7A2Dh
        xor     edi, eax
        add     edx, ebx
        lea     eax, [edi+edx]
        movzx   edx, ch
        movzx   ecx, cl
        mov     edi, ecx
        imul    edi, 7A2Dh
        xor     edi, ecx
        mov     ecx, edx
        imul    ecx, 7A2Dh
        add     edi, eax
        xor     ecx, edx
        add     ecx, edi
        pop     edi
        mov     [esi], ecx
        pop     esi
        pop     ebx
        retn    4
    }
}

}  // namespace NIpse
