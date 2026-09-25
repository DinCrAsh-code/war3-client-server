//============================================================================
//  0x6F4A4B40 - DetachRelation.
//
//  Drop a relation's endpoints without knowing anything else about it: for
//  each endpoint, if this relation is still threaded onto that endpoint's
//  list at all - either link word non-null - detach it.  Endpoint B is only
//  considered when the caller asks for it; endpoint A always is.
//
//  A free `__stdcall` function, not a CAgentRelation member: the relation
//  arrives on the stack rather than in ecx (`mov esi,[esp+4+arg_0]`), ecx is
//  loaded fresh before each of the two detach calls, and it cleans eight
//  bytes.
//
//  The `cmp [esp+arg_4],0` sits ahead of the `push esi` in the shipped code
//  and its branch three instructions later; that is MSVC scheduling the test
//  above the prologue it does not depend on, not a different test.
//
//  Own translation unit: CAgent::SetTrackedTarget (agentsettrackedtarget.cpp)
//  calls it for real.
//============================================================================
#include "agentrelation.h"

void __stdcall DetachRelation(CAgentRelation* relation, int alsoEndpointB)
{
    if (alsoEndpointB != 0)
    {
        if (relation->m_linkB.m_prev != 0 || relation->m_linkB.m_next != 0)
            relation->DetachEndpointB();
    }

    if (relation->m_linkA.m_prev != 0 || relation->m_linkA.m_next != 0)
        relation->DetachEndpointA();
}

//----------------------------------------------------------------------------
//  0x6F4A4C70 - the same "still linked?" gate as DetachRelation's own
//  endpoint-B half, pulled out on its own: `mov ecx,[esp+arg_0]` loads the
//  relation from the stack (a free function, not a member), and the two
//  link-word tests and the call are exactly DetachRelation's own
//  `alsoEndpointB != 0` branch, byte for byte.
//----------------------------------------------------------------------------
void __stdcall DetachEndpointBIfLinked(CAgentRelation* relation)
{
    if (relation->m_linkB.m_prev != 0 || relation->m_linkB.m_next != 0)
        relation->DetachEndpointB();
}
