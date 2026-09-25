//============================================================================
//  0x6F4A6AF0 - CAgentRelation::Establish.
//
//  Break()'s mirror image (agentrelationbreak.cpp), and shaped the same way:
//  a precondition that both endpoints are present, a bit set across the
//  body, the three subscribers told in the shipped order, and the bit
//  cleared on the way out.  Where Break announces '^end' under bits
//  0x10/0x20/0x40 this announces '^beg' under bits 4/1/2 - self first, then
//  endpoint A, then endpoint B - and where Break releases the channel
//  registration this takes one.
//
//  '^beg' is 0x5E626567, the same '^'-prefixed four-character code family
//  agentrelation.h's header describes.
//
//  The registration argument is the relation's own +0x44 word handed over by
//  address.  It is left as the reserved block it has always been rather than
//  declared a CFloat member: CFloat's default constructor zeroes, and
//  CAgentRelation is never constructed here, so a typed member would only
//  add a constructor nothing in the shipped code has.
//
//  `this` stays in ecx across the TakeRegistration call - the shipped code
//  copies it into esi at entry and never reloads ecx, which is why the call
//  sets up only its two stack arguments.
//
//  Own translation unit: CAgentRelation::AttachEndpointA
//  (agentrelationattach.cpp) calls it for real.
//============================================================================
#include "agentrelation.h"

//  '^beg' - the code every announcement out of this function carries.
static const unsigned int kRelationBegin = 0x5E626567;

void CAgentRelation::Establish()
{
    if (m_endpointA == 0)
        return;
    if (m_endpointB == 0)
        return;

    m_flags |= kRelEstablishing;

    if ((m_flags & kRelEstablishTakesReg) != 0)
        TakeRegistration(&m_field44, 0);

    if ((m_flags & kRelBeginTellSelf) != 0)
        PostToSelf(kRelationBegin, 0);

    if ((m_flags & kRelBeginTellA) != 0)
        PostToEndpointA(kRelationBegin, 0, 0);

    if ((m_flags & kRelBeginTellB) != 0)
        PostToEndpointB(kRelationBegin, 0, 0);

    m_flags &= ~kRelEstablishing;
}
