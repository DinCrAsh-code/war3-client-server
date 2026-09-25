//============================================================================
//  0x6F4A5BE0 - SRelationLink::InsertAfter.
//
//  The other half of the unlink DetachEndpointA/B already has
//  (agentrelationdetach.cpp): the relation's own list node is spliced in
//  directly after the head the endpoint handed over.  `this` is that head
//  and the argument is the node, which is why the head arrives in ecx and
//  the node on the stack.
//
//  `m_next` is read twice - once for the copy into the new node, once for
//  the null test - because the store through `node` in between can alias
//  it: nothing tells the compiler that `node` is not this very link, so it
//  has to reload.  The shipped code reloads too, and removing the second
//  read is what a "cleanup" would do here.
//
//  Own translation unit: CAgentRelation::AttachEndpointA
//  (agentrelationattach.cpp) calls it for real.
//============================================================================
#include "agentrelation.h"

void SRelationLink::InsertAfter(SRelationLink* node)
{
    node->m_prev = this;
    node->m_next = m_next;
    if (m_next != 0)
        m_next->m_prev = node;
    m_next = node;
}
