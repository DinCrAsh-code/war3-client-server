//============================================================================
//  0x6F48AF30 - NIpse::CPoReContinuousMod::ComputeChecksum, vtable slot 15.
//  CAgentRelation::ComputeChecksum(hash) via the base, then folds m_rate
//  (+0x50) then m_duration (+0x54) into `*hash` via the same chained
//  per-byte `(b*0x7A2Dh) ^ b` mix documented in Agent/cpoflag_checksum.cpp.
//  Written naked for the same reason as every other member of this
//  family.
//============================================================================
#include "cporecontinuousmod.h"
#include "agentrelation.h"

namespace NIpse {

__declspec(naked) void CPoReContinuousMod::ComputeChecksum(unsigned int* /*hash*/)
{
    __asm
    {
        push    ebx
        push    esi
        mov     esi, [esp+0Ch]
        push    edi
        push    esi
        mov     edi, ecx
        call    CAgentRelation::ComputeChecksum
        mov     ecx, [edi+50h]
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
        mov     ecx, [edi+54h]
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
