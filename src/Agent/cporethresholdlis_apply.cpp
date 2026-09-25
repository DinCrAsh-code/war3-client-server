//============================================================================
//  0x6F4A9A60 - NIpse::CPoReThresholdLis::Apply, vtable slot 3.
//  CAgentRelation::ResetEventBinding(arg) via the base, then m_threshold
//  from `arg`'s own +0x0C field and m_mode from +0x10, then
//  flags |= 0x90400.
//
//  Written naked - a delayed-store shape, same reasoning as
//  Agent/cpofgpulsemod_apply.cpp's own header.
//============================================================================
#include "cporethresholdlis.h"
#include "agentrelation.h"

namespace NIpse {

unsigned int* __fastcall PoReThresholdLisQueryThreshold(unsigned int* out, const void* arg);

struct SPoReThresholdLisSource
{
    int QueryMode();
};

__declspec(naked) void CPoReThresholdLis::Apply(SHandleRegistrationRequest* /*arg*/)
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
        call    PoReThresholdLisQueryThreshold
        mov     eax, [eax]
        mov     ecx, edi
        mov     [esi+50h], eax
        call    SPoReThresholdLisSource::QueryMode
        mov     [esi+54h], eax
        or      dword ptr [esi+4Ch], 90400h
        pop     edi
        pop     esi
        retn    4
    }
}

}  // namespace NIpse
