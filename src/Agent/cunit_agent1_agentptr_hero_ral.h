//============================================================================
//  CAgentPtr_AHer / CAgentPtr_ARal - two more instantiations of the same
//  family cunit_agent1_agentptr_family.h documents in full, called out
//  separately because both AssignChecked bodies below are *directly* in
//  cunit_agent1_worklist.md (0x6F0D2DD0 and 0x6F15FF80) and are scored on
//  their own, not just reached as depth-3 callees of a make-agent function.
//
//  CAgentPtr_AHer's fixed type is 'AHer' - Warcraft III's own Hero rawcode,
//  already named HeroAgileTypeId (Player/playerheroeval.h); it is also what
//  0x6F29B490 (cunit_agent1_makeagent_hero.cpp) builds one of.
//  CAgentPtr_ARal's fixed type is 'ARal' (0x6F027BD0, already
//  GetFourCC_6F027BD0 in misc_trivial_getters.cpp); nothing in this dump
//  needs a converting constructor for it, so it has none.
//============================================================================
#ifndef CUNIT_AGENT1_AGENTPTR_HERO_RAL_H
#define CUNIT_AGENT1_AGENTPTR_HERO_RAL_H

#include "agent.h"

//  0x6F0D2DD0 (AssignChecked, scored here). The ctor at 0x6F0D4BE0 is a
//  different agent's reconstruction (SCheckedSlot_6F2075E0, see
//  cunit_agent1_agentptr_hero_ctor.cpp's header) - not claimed here.
struct CAgentPtr_AHer
{
    CAgentPtr_AHer(CAgent* obj);
    ~CAgentPtr_AHer() { Release(); }

    CAgentPtr_AHer* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F15FF80 (AssignChecked only - no converting-constructor call in this
//  call tree, so none is written).
struct CAgentPtr_ARal
{
    CAgentPtr_ARal* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

#endif
