//============================================================================
//  0x6F4A6B70 - CAgentRelation::AttachEndpointA.
//
//  The other direction of DetachEndpointA (agentrelationdetach.cpp), and
//  the same announce-or-splice choice at the end of it: whatever endpoint A
//  held is dropped, the new endpoint takes its place, the relation is
//  re-established now that both ends may be present, and finally this
//  relation's own list node is put onto the endpoint's list - announced with
//  '^ +l' when the 0x1000 bit is set, spliced in silently otherwise.
//
//  '^ +l' (0x5E202B6C) is one letter off the '^ -l' unlink code
//  DetachEndpointA sends, which is what identifies the pair.
//
//  Own translation unit: CAgent::SetTrackedTarget (agentsettrackedtarget.cpp)
//  calls it for real, twice over.
//============================================================================
#include "agentrelation.h"

//  '^ +l' - the code the link announcement carries.
static const unsigned int kRelationLink = 0x5E202B6C;

void CAgentRelation::AttachEndpointA(void* endpoint, SRelationLink* list)
{
    DetachEndpointA();
    m_endpointA = endpoint;
    Establish();

    if ((m_flags & kRelAnnounceLinkA) != 0)
        PostToEndpointA(kRelationLink, &m_linkA, list);
    else
        list->InsertAfter(&m_linkA);
}
