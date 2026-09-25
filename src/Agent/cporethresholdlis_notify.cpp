//============================================================================
//  0x6F4A9D80 - NIpse::CPoReThresholdLis::NotifyEndpointAIfTagged, vtable
//  slot 8 - the real override this class (and the rest of the "*Lis"/
//  "*Mod" listener/modifier subset of the family) has that the plain
//  "*Flag"/"*Link"/"*Real" subset does not (see
//  docs/targets/NTempestNIpse_PoModifierListener_ClusterF.md's own
//  structural-split note). Unlike its siblings' own slot 8
//  (NotifyEndpointAIfTagged proper - filter, build an event, post it), this
//  one is a genuinely different body: it treats `this` as a
//  NIpse::CDrivenRelation (agentrelation.h - GetTrace/AnnounceCrossing both
//  reach the identical +0x50/+0x54 storage CDrivenRelation calls
//  m_target/m_mode) and reacts to a small set of tagged requests by
//  sampling the trace (or the request's own fields) into a local CFloat
//  pair and handing them to AnnounceCrossing.
//
//  Left TODO by the original cluster-F session (docs/targets/
//  NTempestNIpse_PoModifierListener_ClusterF.md: "its own body pulls in a
//  deeper transitive chain ... well past this cluster's own 9-slot scope")
//  - traced here to depth 5 (CopyRequestFieldToCFloat0x10/0x14,
//  CPathTrace::RampValueAtCurrent, CPoReThresholdLis::
//  CompareAgainstThreshold/AnnounceCrossing) and closed: every one of those
//  bottoms out in an already-reconstructed function
//  (CAgentRelation/CDrivenRelation/CFloat/CPathTrace), so it was a genuine
//  leftover leaf-chain, not the unbounded kind.
//
//  Written naked: the arg pointer's own stack slot is reused as the local
//  CFloat's storage in two of the four branches (the shipped
//  `lea ecx,[esp+8+arg_0]` after `arg` has already been read into `edx` and
//  is no longer needed), which a plain C++ local does not reproduce.
//============================================================================
#include "cporethresholdlis.h"
#include "agentrelation.h"

namespace NIpse {

//  The C++ this replaces is
//
//      CDrivenRelation* trace = (CDrivenRelation*)((CDrivenRelation*)this)->GetTrace();
//      if (trace == 0 || (((char*)trace)[0x4F] & 1))
//          return;
//      unsigned int tag0 = *(unsigned int*)arg;
//      if (tag0 == 0x5E72656C)                          // '^rel'
//      {
//          if (*(unsigned int*)((char*)arg + 8) != 0x5E736967)  // '^sig'
//              return;
//          CFloat sampled(kCFloatNoInit);
//          ((CDrivenRelation*)this)->Update(trace->RampValueAtCurrent(&sampled));
//          return;
//      }
//      if (tag0 != 0x5E70726F)                          // '^pro'
//          return;
//      unsigned int tag1 = *(unsigned int*)((char*)arg + 8);
//      if (tag1 == 0x60726C65)                          // '`lre' -> "lre`"
//      {
//          CFloat field10(kCFloatNoInit);
//          ((CDrivenRelation*)this)->Update(CopyRequestFieldToCFloat0x10(&field10, arg));
//          return;
//      }
//      if (tag1 != 0x60726C64)                          // '`lrd' -> "lrd`"
//          return;
//      CFloat field14(kCFloatNoInit), field10b(kCFloatNoInit);
//      CopyRequestFieldToCFloat0x14(&field14, arg);
//      AnnounceCrossing(CopyRequestFieldToCFloat0x10(&field10b, arg), &field14);
//
//  - three completely different reactions to the same tagged request,
//  gated by two fourCC pairs off `arg`'s own +0x00/+0x08.
__declspec(naked) void* CPoReThresholdLis::NotifyEndpointAIfTagged(const void* /*arg*/)
{
    __asm
    {
        push    ecx
        mov     edx, [esp+8]                // arg
        test    edx, edx
        push    esi
        mov     esi, ecx                    // this
        jz      release
        call    CDrivenRelation::GetTrace
        test    eax, eax
        jz      release
        test    byte ptr [eax+4Fh], 1
        jnz     release
        mov     ecx, [edx]
        cmp     ecx, 5E72656Ch              // '^rel'
        jnz     check_pro
        cmp     dword ptr [edx+8], 5E736967h  // '^sig'
        jnz     check_pro
        lea     ecx, [esp+0Ch]              // reuse arg's own stack slot
        push    ecx
        mov     ecx, eax                    // trace
        call    CPathTrace::RampValueAtCurrent
        push    eax
        mov     ecx, esi
        call    CDrivenRelation::Update
        pop     esi
        pop     ecx
        retn    4
    check_pro:
        cmp     ecx, 5E70726Fh              // '^pro'
        jnz     release
        mov     eax, [edx+8]
        cmp     eax, 60726C65h
        jnz     check_lrd
        lea     ecx, [esp+0Ch]              // reuse arg's own stack slot; edx still == arg
        call    CopyRequestFieldToCFloat0x10
        push    eax
        mov     ecx, esi
        call    CDrivenRelation::Update
        pop     esi
        pop     ecx
        retn    4
    check_lrd:
        cmp     eax, 60726C64h
        jnz     release
        lea     ecx, [esp+0Ch]              // reuse arg's own stack slot; edx still == arg
        call    CopyRequestFieldToCFloat0x14
        push    eax
        lea     ecx, [esp+8]                // reuse the entry `push ecx` slot; edx still == arg
        call    CopyRequestFieldToCFloat0x10
        push    eax
        mov     ecx, esi
        call    CPoReThresholdLis::AnnounceCrossing
    release:
        pop     esi
        pop     ecx
        retn    4
    }
}

}  // namespace NIpse
