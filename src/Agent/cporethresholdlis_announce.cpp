//============================================================================
//  0x6F4A9CF0 - NIpse::CPoReThresholdLis's own threshold-crossing announcer,
//  called from NotifyEndpointAIfTagged (cporethresholdlis_notify.cpp) once
//  it has resolved both candidate points into local CFloats. Gates on
//  CompareAgainstThreshold (cporethresholdlis_compare.cpp) the same way
//  CDrivenRelation::Update (Agent/agentrelationdrive.cpp) gates on
//  AtTarget, and builds the identical 11-dword SAgentTickEventCtx
//  announcement that function already establishes - same '^lis'/'`rtl'
//  identity pair, same subject (`this`), same endpoint-A vtable-slot-0x20
//  dispatch, and the identical CAgentRelation::ReleaseRegistration on
//  arrival - only the gate itself differs (CompareAgainstThreshold instead
//  of AtTarget) and the short-of-target arm calls CDrivenRelation::Recompute
//  (through the identical +0x50/+0x54 storage this class calls
//  m_threshold/m_mode) instead of falling straight through.
//
//  Written naked - identical reasoning to Update's own header: the block
//  layout (the announcement sits behind the branch, not in source order)
//  and the one endpointA-in-esi cache the block never re-reads are not
//  source-addressable, and it is not itself a naked-body transcription
//  defect since this is exactly what Update's own file already documents
//  MSVC doing on the identical shape.
//============================================================================
#include "cporethresholdlis.h"
#include "agentrelation.h"
#include "agenttickrelay.h"

typedef void* (__thiscall *RelationFireFn)(void* self, SAgentTickEventCtx* ctx);

namespace NIpse {

//  The C++ this replaces is
//
//      if (CompareAgainstThreshold(a, b) == 0)
//      {
//          ((CDrivenRelation*)this)->Recompute();
//          return;
//      }
//      ((CAgentRelation*)this)->ReleaseRegistration();
//      SAgentTickEventCtx ctx;
//      ctx.field0 = (void*)0x6072746C;      //  '`rtl'
//      ctx.field4 = (void*)0x6072746C;
//      ctx.eventCode = 0x5E6C6973;          //  '^lis'
//      ctx.agent = (CAgent*)this;
//      ctx.field10 = 0;  ctx.field14 = 0;  ctx.field18 = 0;
//      ctx.field1C = 0;  ctx.field20 = 0;
//      ctx.field28 = -1; ctx.field24 = -1;
//      if (m_endpointA == 0) return;
//      void* target = m_endpointA;
//      void* vtable = *(void**)target;
//      RelationFireFn fn = *(RelationFireFn*)((char*)vtable + 0x20);
//      fn(target, &ctx);
__declspec(naked) void CPoReThresholdLis::AnnounceCrossing(const CFloat* /*a*/, const CFloat* /*b*/)
{
    __asm
    {
        mov     eax, [esp+8]                // b
        sub     esp, 2Ch
        push    esi
        mov     esi, ecx                    // this
        mov     ecx, [esp+34h]              // a
        push    eax
        push    ecx
        mov     ecx, esi
        call    CPoReThresholdLis::CompareAgainstThreshold
        test    eax, eax
        mov     ecx, esi
        jz      short recompute
        call    CAgentRelation::ReleaseRegistration
        mov     eax, 6072746Ch              // '`rtl', the identity and the
        mov     [esp+8], eax                //   event code alike
        mov     [esp+0Ch], eax
        xor     eax, eax
        or      ecx, 0FFFFFFFFh
        cmp     [esi+30h], eax              // m_endpointA
        mov     dword ptr [esp+4], 5E6C6973h    // '^lis'
        mov     [esp+10h], esi              // subject: this
        mov     [esp+14h], eax
        mov     [esp+18h], eax
        mov     [esp+1Ch], eax
        mov     [esp+20h], eax
        mov     [esp+24h], eax
        mov     [esp+2Ch], ecx
        mov     [esp+28h], ecx
        jz      short done
        mov     ecx, [esi+30h]
        mov     edx, [ecx]
        mov     edx, [edx+20h]
        lea     eax, [esp+4]
        push    eax
        call    edx
        pop     esi
        add     esp, 2Ch
        retn    8
    recompute:
        call    CDrivenRelation::Recompute
    done:
        pop     esi
        add     esp, 2Ch
        retn    8
    }
}

}  // namespace NIpse
