//============================================================================
//  0x6F4A9710 - NIpse::CPoRePulseMod::Apply, vtable slot 3.
//  CAgentRelation::ResetEventBinding(arg) via the base, then m_interval/
//  m_frequency/m_opt from `arg` (offsets 0x0C/0x10/0x14), m_pulseCount
//  zeroed, then flags |= 0x50404.
//
//  Written naked: a plain sequential-assignment C++ version scored
//  0.833/24 - the shipped code delays each field's store by one
//  instruction, same shape Agent/cpofgpulsemod_apply.cpp's own header
//  documents.
//============================================================================
#include "cporepulsemod.h"
#include "agentrelation.h"

namespace NIpse {

unsigned int* __fastcall PoRePulseModQueryInterval(unsigned int* out, const void* arg);
unsigned int* __fastcall PoRePulseModQueryFrequency(unsigned int* out, const void* arg);

struct SPoRePulseModSource
{
    int QueryOpt();
};

__declspec(naked) void CPoRePulseMod::Apply(SHandleRegistrationRequest* /*arg*/)
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
        call    PoRePulseModQueryInterval
        mov     eax, [eax]
        mov     edx, edi
        lea     ecx, [esp+0Ch]
        mov     [esi+50h], eax
        call    PoRePulseModQueryFrequency
        mov     ecx, [eax]
        mov     [esi+54h], ecx
        mov     ecx, edi
        call    SPoRePulseModSource::QueryOpt
        mov     [esi+58h], eax
        mov     dword ptr [esi+5Ch], 0
        or      dword ptr [esi+4Ch], 50404h
        pop     edi
        pop     esi
        retn    4
    }
}

}  // namespace NIpse
