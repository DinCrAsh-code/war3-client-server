//============================================================================
//  0x6F4A8500 - NIpse::CPoPoVelocityMod::NotifyEndpointAIfTagged, vtable
//  slot 8. REAL override (CPrRelation's own slot 8 at this position is the
//  shared no-op `0x6F479900`-family placeholder - see cpopointerflis.h's
//  own file header for the same finding on its sibling).
//
//  Filters the incoming context for the '^rel' tag (0x5E72656Ch) and one
//  of two sub-codes: '^beg' (0x5E626567h) or '^end' (0x5E656E64h) - any
//  other tag/sub-code, or a null context, is a no-op. Publishes this
//  object's own velocity (m_velocityX/m_velocityY) to endpoint B's own
//  CPathTrace (GetField0x40_6F4A7840/CPathTrace::PublishVelocityDelta,
//  both already reconstructed - Widget/positionsinksettarget.cpp,
//  Pathfinding/pathtracepublishvelocity.cpp): unchanged on '^beg', negated
//  (both components XORed with the sign bit) on '^end'. On '^beg' only,
//  additionally calls UpdateDurationRegistration() to decide whether
//  m_duration should keep this relation sampled.
//
//  Written naked - the '^end' branch builds the negated velocity pair in a
//  local stack temp and interleaves it with the trace lookup and
//  PublishVelocityDelta's own two stack-pushed arguments in an order a
//  straightforward C++ rewrite is unlikely to reproduce exactly (the same
//  reasoning Agent/agentrelationevents.cpp's own header gives for
//  PostToEndpointA/B/Self, and Agent/cpolklis_slot4.cpp's sibling family
//  gives for this vtable position generally).
//============================================================================
#include "cpopovelocitymod.h"
#include "agentrelation.h"

int __fastcall GetField0x40_6F4A7840(const void* self);

namespace NIpse {

__declspec(naked) void* CPoPoVelocityMod::NotifyEndpointAIfTagged(const void* /*arg*/)
{
    __asm
    {
        mov     eax, [esp+4]
        sub     esp, 8
        test    eax, eax
        push    esi
        mov     esi, ecx
        jz      bail
        cmp     dword ptr [eax], 5E72656Ch
        jnz     bail
        mov     eax, [eax+8]
        cmp     eax, 5E626567h
        jz      onBegin
        cmp     eax, 5E656E64h
        jnz     bail
        mov     ecx, [esi+54h]
        mov     eax, [esi+50h]
        xor     ecx, 80000000h
        push    1
        xor     eax, 80000000h
        lea     edx, [esp+8]
        mov     [esp+0Ch], ecx
        push    edx
        mov     ecx, esi
        mov     [esp+0Ch], eax
        call    GetField0x40_6F4A7840
        mov     ecx, eax
        call    CPathTrace::PublishVelocityDelta
        pop     esi
        add     esp, 8
        retn    4
    onBegin:
        push    1
        lea     eax, [esi+50h]
        push    eax
        call    GetField0x40_6F4A7840
        mov     ecx, eax
        call    CPathTrace::PublishVelocityDelta
        lea     ecx, [esi+58h]
        push    ecx
        mov     ecx, esi
        call    CPoPoVelocityMod::UpdateDurationRegistration
    bail:
        pop     esi
        add     esp, 8
        retn    4
    }
}

}  // namespace NIpse
