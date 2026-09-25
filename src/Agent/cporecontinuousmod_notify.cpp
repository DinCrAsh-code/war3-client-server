//============================================================================
//  0x6F4A9F80 - NIpse::CPoReContinuousMod::NotifyEndpointAIfTagged, vtable
//  slot 8 - the real override this class (and the rest of the "*Lis"/
//  "*Mod" listener/modifier subset of the family) has that the plain
//  "*Flag"/"*Link"/"*Real" subset does not.
//
//  Left TODO by the original cluster-F session
//  (docs/targets/NTempestNIpse_PoModifierListener_ClusterF.md: "calls
//  sub_6F4A9320 -> sub_6F4A91B0, a substantial, unreconstructed
//  event-context builder well past this cluster's own 9-slot scope") -
//  traced here to depth 4 (SHandleObjectRangeImpl::AdjustRate/
//  AnnounceRateChange, Unit/handleobjectrange.h, and
//  ArmDurationRegistration, cporecontinuousmod_arm.cpp) and closed: every
//  callee bottoms out in an already-reconstructed function
//  (CFloat/SPublishedFieldOwner/CAgentRelation), so it was a genuine
//  leftover leaf-chain, not the unbounded kind.
//
//  Filters `arg` on a `'^rel'` tag, then on a second tag at +0x08:
//  `'^end'` adjusts endpoint B's own rate by -m_rate (arriving from the
//  far end), `'^beg'` adjusts it by +m_rate and additionally arms the
//  duration channel off m_duration. Written naked - the arg pointer's own
//  stack slot is reused as the local negated-rate CFloat in the `'^end'`
//  arm, which a plain C++ local does not reproduce.
//============================================================================
#include "cporecontinuousmod.h"
#include "agentrelation.h"
#include "handleobjectrange.h"

int __fastcall GetField0x40_6F4A95A0(const void* self);

namespace NIpse {

//  The C++ this replaces is
//
//      if (arg == 0 || *(unsigned int*)arg != 0x5E72656C)   // '^rel'
//          return;
//      unsigned int tag1 = *(unsigned int*)((char*)arg + 8);
//      if (tag1 == 0x5E626567)                              // '^beg'
//      {
//          CFloat rate = m_rate;
//          ((SHandleObjectRangeImpl*)GetField0x40_6F4A95A0(this))->AdjustRate(&rate);
//          ArmDurationRegistration(&m_duration);
//          return;
//      }
//      if (tag1 != 0x5E656E64)                              // '^end'
//          return;
//      CFloat negatedRate((int)m_rate.m_bits ^ 0x80000000);
//      ((SHandleObjectRangeImpl*)GetField0x40_6F4A95A0(this))->AdjustRate(&negatedRate);
//
//  which the shipped body reaches through the identical shared leaf
//  GetField0x40_6F4A95A0 (Misc/trivialaccessors_04.cpp) both other
//  reconstructed siblings in this cluster already use for their own
//  endpoint B.
__declspec(naked) void* CPoReContinuousMod::NotifyEndpointAIfTagged(const void* /*arg*/)
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
        cmp     eax, 5E626567h              // '^beg'
        jz      arrive_beg
        cmp     eax, 5E656E64h              // '^end'
        jnz     done
        mov     eax, [esi+50h]              // m_rate
        lea     ecx, [esp+8]                // reuse arg's own stack slot
        xor     eax, 80000000h              // -m_rate
        push    ecx
        mov     ecx, esi
        mov     [esp+0Ch], eax
        call    GetField0x40_6F4A95A0
        mov     ecx, eax
        call    SHandleObjectRangeImpl::AdjustRate
        pop     esi
        retn    4
    arrive_beg:
        lea     edx, [esi+50h]              // &m_rate, unnegated
        push    edx
        call    GetField0x40_6F4A95A0
        mov     ecx, eax
        call    SHandleObjectRangeImpl::AdjustRate
        lea     eax, [esi+54h]              // &m_duration
        push    eax
        mov     ecx, esi
        call    CPoReContinuousMod::ArmDurationRegistration
    done:
        pop     esi
        retn    4
    }
}

}  // namespace NIpse
