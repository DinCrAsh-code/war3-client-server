//============================================================================
//  0x6F4A9760 - NIpse::CPoRePulseMod::NotifyEndpointAIfTagged, vtable
//  slot 8.
//
//  Filters `arg` on a `'^rel'` tag, dispatches on a second tag at +0x08:
//  `'^beg'` drains a pending countdown at +0x58 into +0x5C and, if it was
//  non-zero, calls CAgentRelation::TakeRegistration on +0x54 before
//  falling into the `'^sig'` case; `'^sig'` calls
//  SHandleObjectRangeImpl::AddToValue(&m_interval) on endpoint B (through
//  GetField0x40_6F4A9700, cporepulsemod_getendpointbraw.cpp), then
//  decrements +0x5C and calls CAgentRelation::ReleaseRegistration once it
//  reaches zero. Written naked - same reasoning as
//  Agent/cpofgpulsemod_notify.cpp's own sibling.
//============================================================================
#include "cporepulsemod.h"
#include "agentrelation.h"
#include "handleobjectrange.h"

int __fastcall GetField0x40_6F4A9700(const void* self);

namespace NIpse {

__declspec(naked) void* CPoRePulseMod::NotifyEndpointAIfTagged(const void* /*arg*/)
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
        mov     eax, [esi+58h]
        test    eax, eax
        mov     [esi+5Ch], eax
        jz      done
        push    0
        lea     eax, [esi+54h]
        push    eax
        call    CAgentRelation::TakeRegistration
    sig:
        lea     ecx, [esi+50h]
        push    ecx
        mov     ecx, esi
        call    GetField0x40_6F4A9700
        mov     ecx, eax
        call    SHandleObjectRangeImpl::AddToValue
        add     dword ptr [esi+5Ch], 0FFFFFFFFh
        jnz     done
        mov     ecx, esi
        call    CAgentRelation::ReleaseRegistration
    done:
        pop     esi
        retn    4
    }
}

}  // namespace NIpse
