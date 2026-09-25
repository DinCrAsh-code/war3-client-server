//============================================================================
//  0x6F4A95B0 - NIpse::CPoReContinuousMod::Apply, vtable slot 3.
//  CAgentRelation::ResetEventBinding(arg) via the base, then m_rate from
//  `arg`'s own +0x0C field and m_duration from +0x10 (each via its own
//  null-safe, CFloatZero-default leaf), then flags |= 0x40044.
//
//  Written naked: a plain sequential-assignment C++ version scored
//  0.850/20 - the shipped code delays each field's store by one
//  instruction (interleaved with the next call's own argument setup),
//  same shape Agent/cpofgpulsemod_apply.cpp's own header documents.
//============================================================================
#include "cporecontinuousmod.h"
#include "agentrelation.h"

namespace NIpse {

unsigned int* __fastcall PoReContinuousModQueryRate(unsigned int* out, const void* arg);
unsigned int* __fastcall PoReContinuousModQueryDuration(unsigned int* out, const void* arg);

__declspec(naked) void CPoReContinuousMod::Apply(SHandleRegistrationRequest* /*arg*/)
{
    __asm
    {
        push    esi
        push    edi
        mov     edi, [esp+0Ch]
        push    edi
        mov     esi, ecx
        call    CAgentRelation::ResetEventBinding
        mov     edx, edi
        lea     ecx, [esp+0Ch]
        call    PoReContinuousModQueryRate
        mov     eax, [eax]
        mov     edx, edi
        lea     ecx, [esp+0Ch]
        mov     [esi+50h], eax
        call    PoReContinuousModQueryDuration
        mov     ecx, [eax]
        mov     [esi+54h], ecx
        or      dword ptr [esi+4Ch], 40044h
        pop     edi
        pop     esi
        retn    4
    }
}

}  // namespace NIpse
