//============================================================================
//  0x6F4A68E0 / 0x6F4A6890 - release and take the channel registration.
//
//  Releasing marks the old registration retired (0x10000 in its own flags
//  word) and drops it, and then drops whichever endpoints the relation's
//  flags say the registration was holding - B first, then A as a tail call.
//
//  Taking one retires whatever was there, asks the global time sync for this
//  relation's channel and registers a sample on it, and finally - when the
//  relation wants sampling at all - marks the new registration for it.
//============================================================================
#include "agentrelation.h"

//----------------------------------------------------------------------------
//  0x6F4A68E0
//----------------------------------------------------------------------------
void CAgentRelation::ReleaseRegistration()
{
    SSyncRegistration* reg = m_registration;
    if (reg == 0)
        return;

    reg->m_flags |= kRelWantSample;
    m_registration = 0;

    if ((m_flags & kRelDropAOnRelease) != 0)
        DetachEndpointB();
    if ((m_flags & kRelDropBOnRelease) != 0)
        DetachEndpointA();
}

//----------------------------------------------------------------------------
//  0x6F4A6890
//----------------------------------------------------------------------------
void CAgentRelation::TakeRegistration(const CFloat* value, int time)
{
    SSyncRegistration* old = m_registration;
    if (old != 0)
        old->m_flags |= kRelWantSample;

    SSyncRegistration* reg =
        (SSyncRegistration*)((CSyncChannel*)g_pTimeSync->GetChannel(
            (const CHandleObject*)this))->Register(this, &time, value);

    m_registration = reg;

    //  The 0x10000 bit read as *byte* +0x4E's bit 0, and the same 1 used
    //  for the store: the shipped code parks a 1 in ecx, tests the byte with
    //  it and then ORs it in, which a 32-bit `test [flags], 0x10000` does
    //  not produce.
    if ((m_bits.m_flagByte4E & 1) != 0)
        reg->m_flags |= 1;
}
