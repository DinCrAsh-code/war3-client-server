//============================================================================
//  0x6F48D020 - NIpse::CPoFlag::ComputeChecksum, vtable slot 15. Calls the
//  base's own CRlProperty::ComputeChecksum(hash) first, then folds
//  m_value's own four bytes into `*hash`, one byte at a time, through the
//  same `(b*0x7A2Dh) ^ b` mix per byte Agent/cpofglis_checksum_dump.cpp's
//  own CPoFgLis::ComputeChecksum already documents for a single added
//  field - byte3 folded straight into `*hash` (store), reload `*hash`,
//  then byte2, byte0 and byte1 mixed and summed with the reloaded hash
//  before one final store.
//
//  Written naked for the same reason as CPoFgLis's own: two rewrites as
//  ordinary C++ were tried there and both scored WORSE than the naked
//  transcription (Agent/cpofglis_checksum_dump.cpp's own header has the
//  full investigation) because the optimiser either merges the two *hash
//  updates the shipped code keeps genuinely separate, or reorders two
//  independent byte extractions relative to the one dependency that must
//  stay ordered. Not re-litigated per class; same shape, same conclusion.
//============================================================================
#include "cpoflag.h"
#include "rlproperty.h"

namespace NIpse {

__declspec(naked) void CPoFlag::ComputeChecksum(unsigned int* /*hash*/)
{
    __asm
    {
        push    esi
        mov     esi, [esp+8]
        push    edi
        push    esi
        mov     edi, ecx
        call    CRlProperty::ComputeChecksum
        mov     ecx, [edi+78h]
        mov     eax, ecx
        shr     eax, 10h
        movzx   edx, ah
        mov     edi, edx
        imul    edi, 7A2Dh
        xor     edi, edx
        add     [esi], edi
        mov     edx, [esi]
        movzx   eax, al
        mov     edi, eax
        imul    edi, 7A2Dh
        xor     edi, eax
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
        retn    4
    }
}

}  // namespace NIpse
