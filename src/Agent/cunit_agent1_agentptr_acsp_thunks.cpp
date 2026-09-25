//============================================================================
//  CAgentPtr_ACsp's own ctor (0x6F1E5600) and AssignChecked (0x6F1DA890) -
//  two hops out of the sweep that reached MakeCheckedAgent_ACsp
//  (cunit_agent1_makeagent.cpp), so declared and self-redirect-thunked
//  here rather than reconstructed, the same collision-avoidance reasoning
//  cunit_agent8_abilityfactories.cpp's own out-of-slice pairs already use.
//============================================================================
#include "cunit_agent1_agentptr_family.h"

__declspec(naked) CAgentPtr_ACsp::CAgentPtr_ACsp(CAgent*)
{
    __asm { mov eax, 06F1E5600h }
    __asm { jmp eax }
}

__declspec(naked) CAgentPtr_ACsp* CAgentPtr_ACsp::AssignChecked(CAgent*)
{
    __asm { mov eax, 06F1DA890h }
    __asm { jmp eax }
}
