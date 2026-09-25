//============================================================================
//  0x6F275830 - one more instantiation of the "type-checked smart pointer to
//  a CAgent-derived object" family (agenttypedslots.h), same shape as
//  SCheckedUnitSlot::Assign (unittypedassign.cpp).
//
//  Its own required-type getter, 0x6F028940, is below this agent-6 slice's
//  own BFS cut and is itself a worklist entry for a different concurrent
//  agent's slice (cunit_agent5_worklist.md) - so, per this whole family's
//  "delete each thunk as the real body lands" rule, it stays thunked here
//  under an address-only name rather than duplicated under a guessed one.
//============================================================================
#include "agent.h"
#include "agiletype.h"

namespace {

//  0x6F028940 - thunked; owned by a different concurrent agent's slice.
extern "C" __declspec(naked) unsigned int GetRequiredTag_6F275830()
{
    __asm { mov eax, 0x6F028940 }
    __asm { retn }
}

}  // namespace

struct SCheckedSlot_6F275830
{
    SCheckedSlot_6F275830* Assign(CAgent* candidate);
    CAgent* m_value;
};

SCheckedSlot_6F275830* SCheckedSlot_6F275830::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate &&
        AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                GetRequiredTag_6F275830()))
        checked = candidate;
    else
        checked = 0;

    CAgent* old = m_value;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (candidate)
                candidate->m_refcount++;
        }
        m_value = checked;
    }

    return this;
}
