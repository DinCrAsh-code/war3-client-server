//============================================================================
//  0x6F4A7D20 - NIpse::CPoPoVelocityMod::Apply, vtable slot 3.
//  CAgentRelation::ResetEventBinding(arg) via the base, then
//  m_velocityX/m_velocityY as one pair from `arg`'s own +0x0C/+0x10 fields
//  (PoPoVelocityModQueryVelocity, a hidden-return-pointer leaf returning
//  both CFloats at once - default {g_CFloatZero, g_CFloatZero}), then
//  m_duration from `arg`'s own +0x14 field (PoPoVelocityModQueryDuration,
//  default g_CFloatZero), then flags |= 0x40044.
//
//  Written naked - same "shipped code delays each store, interleaved with
//  the next call's own argument setup" reasoning as every other Apply in
//  this family (Agent/cporecontinuousmod_apply.cpp's own header).
//============================================================================
#include "cpopovelocitymod.h"
#include "agentrelation.h"

namespace NIpse {

void* __fastcall PoPoVelocityModQueryVelocity(void* out, const void* arg);
unsigned int* __fastcall PoPoVelocityModQueryDuration(unsigned int* out, const void* arg);

__declspec(naked) void CPoPoVelocityMod::Apply(SHandleRegistrationRequest* /*arg*/)
{
    __asm
    {
        sub     esp, 8
        push    esi
        push    edi
        mov     edi, [esp+14h]
        push    edi
        mov     esi, ecx
        call    CAgentRelation::ResetEventBinding
        mov     edx, edi
        lea     ecx, [esp+8]
        call    PoPoVelocityModQueryVelocity
        mov     ecx, [eax]
        mov     [esi+50h], ecx
        mov     edx, [eax+4]
        mov     [esi+54h], edx
        mov     edx, edi
        lea     ecx, [esp+14h]
        call    PoPoVelocityModQueryDuration
        mov     eax, [eax]
        mov     [esi+58h], eax
        or      dword ptr [esi+4Ch], 40044h
        pop     edi
        pop     esi
        add     esp, 8
        retn    4
    }
}

}  // namespace NIpse
