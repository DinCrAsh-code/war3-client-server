//============================================================================
//  0x6F4A6750 - decide whether this relation should hold a channel
//  registration, and take one if so.
//
//  Not a function IDA knows about: it is reached only by `jmp` from
//  DetachEndpointA and DetachEndpointB, and IDA has modelled it as a *tail
//  chunk* of both rather than as a function of its own - so it has no
//  `### ... @ 0x...` heading anywhere, no funcmap entry, and no score.  It
//  is still a real function in the shipped binary (its own push/pop
//  prologue and epilogue are right there), and both callers' listings carry
//  a copy of it after their own `retn`, which is why they score around 52
//  of 74 however faithfully the body above it is reconstructed.
//
//  Which endpoints have to be *absent* before a registration is wanted is
//  three bits of the flags word: 0x10000000 asks for both gone, 0x20000000
//  for A gone, 0x40000000 for B gone, and with none of the three set the
//  relation never registers at all.
//============================================================================
#include "agentrelation.h"
#include "CFloat.h"

void CAgentRelation::AcquireChannel()
{
    if (m_channelReg != 0)
        return;

    unsigned int flags = m_flags;
    if ((flags & kRelNeedsAny) == 0)
        return;

    if ((flags & kRelNeedsBoth) != 0)
    {
        if (m_endpointA != 0)
            return;
        if (m_endpointB != 0)
            return;
    }
    else if ((flags & kRelNeedsA) == 0 || m_endpointA != 0)
    {
        if ((flags & kRelNeedsB) == 0)
            return;
        if (m_endpointB != 0)
            return;
    }

    int time = 0;
    m_channelReg = (SSyncRegistration*)
        ((CSyncChannel*)g_pTimeSync->GetChannel(
            (const CHandleObject*)this))->Register(this, &time, &g_CFloatZero);
}
