//============================================================================
//  0x6F4A8B80 - NIpse::CPoFgPulseMod::Apply, vtable slot 3.
//  CAgentRelation::ResetEventBinding(arg) via the base, then four fields
//  from `arg` (offsets 0x0C/0x10/0x14/0x18, each via its own null-safe
//  leaf), m_playfield zeroed, then flags |= 0x50404.
//
//  Written naked: a plain sequential-assignment C++ version scored
//  0.857/24 - the shipped code stores each call's own result ONE
//  instruction late (interleaved with the next call's own argument setup)
//  rather than immediately after the call that produced it, which a
//  straightforward statement-by-statement rewrite does not reproduce.
//============================================================================
#include "cpofgpulsemod.h"
#include "agentrelation.h"

namespace NIpse {

//  Forward declarations matching the actual leaves (thiscall-shaped, so
//  declared as struct methods per this cluster's own convention).
struct SPoFgPulseModSource
{
    int QueryFrequency();
    int QueryDuration();
    int QueryOpt();
};

unsigned int* __fastcall PoFgPulseModQueryPlayfield(unsigned int* out, const void* arg);

__declspec(naked) void CPoFgPulseMod::Apply(SHandleRegistrationRequest* /*arg*/)
{
    __asm
    {
        push    esi
        push    edi
        mov     edi, [esp+0Ch]
        push    edi
        mov     esi, ecx
        call    CAgentRelation::ResetEventBinding
        mov     ecx, edi
        call    SPoFgPulseModSource::QueryFrequency
        mov     [esi+50h], eax
        call    SPoFgPulseModSource::QueryDuration
        mov     edx, edi
        lea     ecx, [esp+0Ch]
        mov     [esi+54h], eax
        call    PoFgPulseModQueryPlayfield
        mov     eax, [eax]
        mov     ecx, edi
        mov     [esi+58h], eax
        call    SPoFgPulseModSource::QueryOpt
        mov     [esi+5Ch], eax
        mov     dword ptr [esi+60h], 0
        or      dword ptr [esi+4Ch], 50404h
        pop     edi
        pop     esi
        retn    4
    }
}

}  // namespace NIpse
