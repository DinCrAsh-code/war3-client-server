//============================================================================
//  0x6F4A6BC0 - CAgentRelation::AttachEndpointB, the mirror image of
//  AttachEndpointA (agentrelationattach.cpp) for the other endpoint: point
//  endpoint B at `endpoint`, break whatever endpoint B already was
//  (Establish's own m_endpointA-independent teardown - Establish itself
//  handles re-marking the relation live), and either announce the new link
//  to endpoint B ('^+l', 0x5E202B6C) or splice this relation's own m_linkB
//  onto `list` silently, chosen by the 0x2000 flag bit - the same
//  announce-or-splice shape AttachEndpointA's own 0x1000 bit picks between,
//  one bit further up the flags word.
//
//  Own translation unit: DetachEndpointB, Establish and PostToEndpointB are
//  all real calls (agentrelation.h).
//============================================================================
#include "agentrelation.h"

const unsigned int kRelAnnounceLinkB = 0x00002000;

void CAgentRelation::AttachEndpointB(void* endpoint, SRelationLink* list)
{
    DetachEndpointB();
    m_endpointB = endpoint;
    Establish();

    if (m_flags & kRelAnnounceLinkB)
    {
        PostToEndpointB(0x5E202B6C, &m_linkB, list);
        return;
    }
    list->InsertAfter(&m_linkB);
}
