//============================================================================
//  0x6F4A8D60 - NIpse::CPoFgPulseMod::NotifyEndpointAIfTagged, vtable
//  slot 8.
//
//  Filters `arg` on a `'^rel'` tag (its own +0x00), then dispatches on a
//  second tag at +0x08: `'^beg'` (0x5E626567) drains a pending countdown
//  at +0x5C into +0x60 and, if it was non-zero, calls
//  CAgentRelation::TakeRegistration on +0x58 before falling into the
//  `'^sig'` case; `'^sig'` (0x5E736967) XORs a bitmask derived from
//  m_mask/m_delta (+0x50/+0x54) into endpoint B's own +0x78 field
//  (through GetEndpointBRaw, cpofgpulsemod_getendpointbraw.cpp) and
//  notifies it via SPublishedFieldOwner::NotifyField78Changed
//  (sub_6F4A8980), then decrements +0x60 and calls
//  CAgentRelation::ReleaseRegistration once it reaches zero.
//
//  Written naked: the shipped body interleaves the tag filter, the
//  conditional TakeRegistration and the endpoint-B read/xor/write/notify
//  in a specific register order (this call tree's own established
//  precedent for this exact family - Agent/cpolklis_notify.cpp's own
//  header already gives the general reasoning).
//============================================================================
#include "cpofgpulsemod.h"
#include "agentrelation.h"

//  Declared to match Misc/handlefieldnotify.cpp's own definition exactly
//  (same class/method name and parameter types => same mangled symbol) -
//  that struct is file-local there, so it cannot be #included.
struct SPublishedFieldOwner
{
    void NotifyField78Changed(int oldValue, void* listener);
};

namespace NIpse {

__declspec(naked) void* CPoFgPulseMod::NotifyEndpointAIfTagged(const void* /*arg*/)
{
    __asm
    {
        mov     eax, [esp+4]
        test    eax, eax
        push    esi
        mov     esi, ecx
        jz      done
        cmp     dword ptr [eax], 5E72656Ch
        jnz     done
        mov     eax, [eax+8]
        cmp     eax, 5E626567h
        jz      beg
        cmp     eax, 5E736967h
        jz      sig
        pop     esi
        retn    4
    beg:
        mov     eax, [esi+5Ch]
        test    eax, eax
        mov     [esi+60h], eax
        jz      done
        push    0
        lea     eax, [esi+58h]
        push    eax
        call    CAgentRelation::TakeRegistration
    sig:
        push    ebx
        mov     ebx, [esi+54h]
        test    ebx, ebx
        mov     ecx, esi
        jnz     haveDelta
        call    CPoFgPulseMod::GetEndpointBRaw
        mov     ecx, [eax+78h]
        mov     edx, [esi+50h]
        xor     [eax+78h], edx
        push    ebx
        push    ecx
        mov     ecx, eax
        call    SPublishedFieldOwner::NotifyField78Changed
        jmp     decCount
    haveDelta:
        push    edi
        call    CPoFgPulseMod::GetEndpointBRaw
        mov     edi, [eax+78h]
        call    CPoFgPulseMod::GetEndpointBRaw
        mov     edx, [esi+50h]
        mov     ecx, [eax+78h]
        add     ebx, edi
        and     ebx, edx
        not     edx
        and     edx, edi
        push    0
        or      ebx, edx
        push    ecx
        mov     ecx, eax
        mov     [eax+78h], ebx
        call    SPublishedFieldOwner::NotifyField78Changed
        pop     edi
    decCount:
        add     dword ptr [esi+60h], 0FFFFFFFFh
        pop     ebx
        jnz     done
        mov     ecx, esi
        call    CAgentRelation::ReleaseRegistration
    done:
        pop     esi
        retn    4
    }
}

}  // namespace NIpse
