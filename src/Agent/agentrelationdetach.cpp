//============================================================================
//  0x6F4A67D0 / 0x6F4A6830 - drop one endpoint.
//
//  Two copies of one function over the two endpoint slots, and two functions
//  in the shipped build.  Each unlinks this relation from its endpoint's
//  list - announcing the unlink to that endpoint first, with the list node's
//  own address as the event's detail, when the endpoint's "announce" bit is
//  set, and splicing it out silently otherwise - then, if the endpoint is
//  still there, breaks the relation, clears the endpoint and re-decides the
//  channel registration.
//
//  The tail is a `jmp` to AcquireChannel (agentrelationchannel.cpp), which
//  IDA has attached to *both* of these as a tail chunk rather than treating
//  as a function; see that file for why both score around 52 of 74.
//============================================================================
#include "agentrelation.h"

//  '^ -l' - the code the unlink announcement carries.
static const unsigned int kRelationUnlink = 0x5E202D6C;

//----------------------------------------------------------------------------
//  0x6F4A67D0
//----------------------------------------------------------------------------
void CAgentRelation::DetachEndpointA()
{
    if ((m_flags & kRelAnnounceUnlinkA) != 0)
    {
        PostToEndpointA(kRelationUnlink, &m_linkA, 0);
    }
    else
    {
        SRelationLink* prev = m_linkA.m_prev;
        SRelationLink* next = m_linkA.m_next;
        if (prev != 0)
            prev->m_next = next;
        if (next != 0)
            next->m_prev = prev;
        m_linkA.m_prev = 0;
        m_linkA.m_next = 0;
    }

    if (m_endpointA != 0)
    {
        Break();
        m_endpointA = 0;
        AcquireChannel();
    }
}

//----------------------------------------------------------------------------
//  0x6F4A6830
//----------------------------------------------------------------------------
void CAgentRelation::DetachEndpointB()
{
    if ((m_flags & kRelAnnounceUnlinkB) != 0)
    {
        PostToEndpointB(kRelationUnlink, &m_linkB, 0);
    }
    else
    {
        SRelationLink* prev = m_linkB.m_prev;
        SRelationLink* next = m_linkB.m_next;
        if (prev != 0)
            prev->m_next = next;
        if (next != 0)
            next->m_prev = prev;
        m_linkB.m_prev = 0;
        m_linkB.m_next = 0;
    }

    if (m_endpointB != 0)
    {
        Break();
        m_endpointB = 0;
        AcquireChannel();
    }
}
