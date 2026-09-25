//============================================================================
//  CAgentPtr_Buff - another instantiation of the CAgentPtr family (agent.h,
//  CAgentPtr::AssignChecked), this one fixed to 'buff' (0x62756666),
//  already named GetTypeTag_6F0D1AB0 in Misc/trivialaccessors_02_03.cpp -
//  grepped for and reused rather than re-declared under a second name.
//
//  0x6F03C050 (the converting constructor) is directly in
//  cunit_agent1_worklist.md and is scored on its own; 0x6F02B660
//  (AssignChecked) is a depth-3 callee of it, so it carries no claimed
//  verdict here - it is still a real, address-carrying transcription
//  rather than a thunk, because AssignChecked's own shape
//  (CAgentPtr::AssignChecked, agent.cpp) is already established and cheap
//  to reuse exactly.
//============================================================================
#include "agent.h"
#include "agiletype.h"
#include <excpt.h>   // AbnormalTermination()

//  0x6F0D1AB0 - Misc/trivialaccessors_02_03.cpp.  `__fastcall(void* self)`
//  with `self` unused, the uniform shape every GetTypeTag_<addr> leaf in
//  that batch has - not the zero-argument declaration this file's own
//  call site would otherwise suggest.
unsigned int __fastcall GetTypeTag_6F0D1AB0(void* self);

//  0x6F471910 - agiletype.h.
int __fastcall AgileTypeIsDerivedFrom(unsigned int type, unsigned int base);

struct CAgentPtr_Buff
{
    //  0x6F03C050.
    CAgentPtr_Buff(CAgent* obj);
    ~CAgentPtr_Buff() { Release(); }

    //  0x6F02B660.
    CAgentPtr_Buff* AssignChecked(CAgent* obj);

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

//  0x6F03C050
CAgentPtr_Buff::CAgentPtr_Buff(CAgent* obj)
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

//  0x6F02B660
CAgentPtr_Buff* CAgentPtr_Buff::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetTypeTag_6F0D1AB0(obj)))
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
