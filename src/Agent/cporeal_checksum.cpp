//============================================================================
//  0x6F48C410 - NIpse::CPoReal::ComputeChecksum, vtable slot 15.
//  CRlProperty::ComputeChecksum(hash) via the base, then folds all four of
//  this class's own fields (+0x78/+0x7C/+0x80/+0x84, in that offset order)
//  into `*hash` via the same chained per-byte `(b*0x7A2Dh) ^ b` mix
//  Agent/cpoflag_checksum.cpp's own single-field version documents. Written
//  naked for the same reason as every other member of this family - see
//  that file's own header for the imul-rendering investigation this does
//  not re-litigate. Stack offset (`[esp+10h]`) confirmed against this
//  function's own `raw_bytes`, not just IDA's symbolic rendering.
//============================================================================
#include "cporeal.h"
#include "rlproperty.h"

namespace NIpse {

__declspec(naked) void CPoReal::ComputeChecksum(unsigned int* /*hash*/)
{
    __asm
    {
        push    ebx
        push    ebp
        push    esi
        mov     esi, [esp+10h]
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
        add     eax, ebx
        movzx   ecx, cl
        mov     edx, ecx
        imul    edx, 7A2Dh
        xor     edx, ecx
        add     edx, eax
        mov     [esi], edx
        mov     ecx, [edi+7Ch]
        mov     eax, ecx
        shr     eax, 10h
        movzx   ebx, ah
        mov     ebp, ebx
        imul    ebp, 7A2Dh
        xor     ebp, ebx
        movzx   eax, al
        mov     ebx, eax
        imul    ebx, 7A2Dh
        xor     ebx, eax
        add     edx, ebp
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
        mov     ecx, [edi+80h]
        mov     eax, ecx
        shr     eax, 10h
        movzx   ebx, ah
        mov     ebp, ebx
        imul    ebp, 7A2Dh
        xor     ebp, ebx
        movzx   eax, al
        mov     ebx, eax
        imul    ebx, 7A2Dh
        add     edx, ebp
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
        mov     ecx, [edi+84h]
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
        pop     ebp
        pop     ebx
        retn    4
    }
}

}  // namespace NIpse
