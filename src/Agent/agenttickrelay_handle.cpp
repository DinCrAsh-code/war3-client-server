//============================================================================
//  0x6F4AB150 / 0x6F4AA560 - CAgentTickRelay::ResolveAgentHandle/Arm (see
//  agenttickrelay.h).  Own TU, separate from ReleasePendingA/B
//  (agenttickrelay_pending.cpp): Arm reaches them through real `call`
//  instructions in the target listing.
//============================================================================
#include "agenttickrelay.h"
#include "agent.h"

CAgent* __thiscall CAgentTickRelay::ResolveAgentHandle(int)
{
    if (!(m_flags & 1))
        return 0;
    return (CAgent*)LookupHandle(m_agentHandle, m_agentTypeTag);
}

void __thiscall CAgentTickRelay::Arm(int arm, int releaseA, int releaseB)
{
    //  Two separate `if (arm)` regions, not one: the shipped code re-tests
    //  `arm` (kept in edx) after the two calls rather than remembering the
    //  branch.  MSVC folds the second test whichever way this is spelled
    //  (nested-if, `arm && releaseX`, and a hoisted-flags local were all
    //  measured at the identical 17-instruction stream), so the shape is
    //  written the way the target reads.
    if (arm)
    {
        if (releaseA)
            ReleasePendingA();
        if (releaseB)
            ReleasePendingB();
    }

    if (arm)
        m_flags |= 2;
    else
        m_flags &= ~2u;
}
