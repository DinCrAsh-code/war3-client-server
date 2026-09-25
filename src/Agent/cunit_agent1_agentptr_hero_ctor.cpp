//============================================================================
//  CAgentPtr_AHer's converting constructor. 0x6F0D4BE0 is NOT reconstructed
//  here - it turned out to already be cunit_agent2's own worklist item,
//  properly claimed and scored there as SCheckedSlot_6F2075E0's own
//  constructor (tools/funcmap.py). This local definition exists purely so
//  MakeCheckedAgent_AHer (cunit_agent1_makeagent_hero.cpp, which needs a
//  CAgentPtr_AHer local) compiles; it carries no funcmap row of its own and
//  is not scored/claimed under this address - same reasoning as
//  cunit_agent1_agentptr_ctors.cpp, whose body this one shares exactly.
//============================================================================
#include "cunit_agent1_agentptr_hero_ral.h"
#include <excpt.h>   // AbnormalTermination()

//  0x6F0D4BE0
CAgentPtr_AHer::CAgentPtr_AHer(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}
