//============================================================================
//  0x6F4A79A0 - NIpse::CPoPoInterfLis::Apply, vtable slot 3.
//  CAgentRelation::ResetEventBinding(arg) via the base, then m_radius from
//  `arg`'s own +0x0C field (via the new leaf PoPoInterfLisQueryRadius,
//  null-safe, g_CFloatZero default), m_opt = 0, flags |= 0x190400, and
//  finally a call into PoPoInterfLisSetField44AndSample (0x6F4A7980) which
//  is NOT array-related - it stamps CAgentRelation::m_field44 (+0x44) from
//  a fixed global and takes a fresh channel registration off its own
//  address, the same "resample on Apply" shape a few other siblings in
//  this wider family end their own Apply with.
//
//  Written naked - the shipped code delays each field's store, interleaved
//  with the next call's own argument setup, the same shape
//  Agent/cpofgpulsemod_apply.cpp's/cporecontinuousmod_apply.cpp's own
//  headers already document for this family.
//============================================================================
#include "cpopointerflis.h"
#include "agentrelation.h"

//  0x6FAB7770 - a fixed global CAgentRelation::m_field44's own value is
//  stamped from at Apply time. Nothing else in this call tree names it
//  further; kept as a raw dword the same way g_unk6FAB7764/g_unk6FAB778C
//  (Agent/proximitymap_initgrid.cpp/agentbasechain_staticpools.h) are.
//  Declared at file scope (not inside namespace NIpse) to match the plain
//  `?g_unk6FAB7770@@3IA` symbol funcmap.DATA's row expects - every other
//  g_unk global in this codebase is a bare global for the same reason.
extern unsigned int g_unk6FAB7770;

namespace NIpse {

unsigned int* __fastcall PoPoInterfLisQueryRadius(unsigned int* out, const void* arg);

__declspec(naked) void CPoPoInterfLis::Apply(SHandleRegistrationRequest* /*arg*/)
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
        call    PoPoInterfLisQueryRadius
        mov     eax, [eax]
        mov     [esi+50h], eax
        mov     dword ptr [esi+54h], 0
        or      dword ptr [esi+4Ch], 190400h
        push    offset g_unk6FAB7770
        mov     ecx, esi
        call    CPoPoInterfLis::SetField44AndSample
        pop     edi
        pop     esi
        retn    4
    }
}

__declspec(naked) void CPoPoInterfLis::SetField44AndSample(const void* /*source*/)
{
    __asm
    {
        mov     edx, [esp+4]
        mov     edx, [edx]
        lea     eax, [ecx+44h]
        push    0
        push    eax
        mov     [eax], edx
        call    CAgentRelation::TakeRegistration
        retn    4
    }
}

}  // namespace NIpse
