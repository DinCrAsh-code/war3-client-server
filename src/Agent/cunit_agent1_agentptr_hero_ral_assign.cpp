//============================================================================
//  CAgentPtr_AHer::AssignChecked (0x6F0D2DD0) and CAgentPtr_ARal::AssignChecked
//  (0x6F15FF80) - both directly in cunit_agent1_worklist.md, both the exact
//  same instruction stream as CAgentPtr::AssignChecked (agent.cpp) with only
//  the fixed-type call operand differing:
//
//      push ebx/esi/edi; edi = candidate; ebx = this; if (edi) {
//          esi = <fixed type>(); ecx = edi; edx = candidate's own
//          GetAgileTypeId() slot; call edx; edx = esi; ecx = eax;
//          call AgileTypeIsDerivedFrom; esi = eax ? edi : 0; } else esi = 0;
//      <release old, addref new, store, same as every other instantiation>
//
//  which is what makes both of them line up instruction for instruction with
//  the already-reconstructed AssignChecked rather than needing their own
//  transcription.
//============================================================================
#include "cunit_agent1_agentptr_hero_ral.h"
#include "agiletype.h"
#include "playerheroeval.h"   // HeroAgileTypeId - 0x6F2075E0

//  0x6F027BD0 - misc_trivial_getters.cpp.
unsigned int GetFourCC_6F027BD0();

//  0x6F471910 - agiletype.h.
int __fastcall AgileTypeIsDerivedFrom(unsigned int type, unsigned int base);

//  0x6F0D2DD0
CAgentPtr_AHer* CAgentPtr_AHer::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), HeroAgileTypeId()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F15FF80
CAgentPtr_ARal* CAgentPtr_ARal::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F027BD0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}
