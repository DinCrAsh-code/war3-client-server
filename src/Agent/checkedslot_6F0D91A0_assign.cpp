//============================================================================
//  0x6F275630 - SCheckedSlot_6F0D91A0::Assign, from the CUnit vtable
//  closure walk (cunit_agent2_worklist.md).  Same shape as
//  checkedtaskslot_assign.cpp/taskactiontypedassign.cpp: checked against
//  0x6F0D91A0's own rawcode ('AUbu' MSB-first, `mov eax,41556275h`), named
//  by the checkedslot_batch2526.h convention (class named after its own
//  fixed-type-id getter's address).
//============================================================================
#include "agent.h"
#include "agiletype.h"

unsigned int GetConstant_6F0D91A0();   //  0x6F0D91A0 - 'AUbu'. misc_rawcode_getters.cpp.

struct SCheckedSlot_6F0D91A0
{
    //  0x6F275630 - this file.
    SCheckedSlot_6F0D91A0* Assign(CAgent* candidate);

    CAgent* m_value;
};

SCheckedSlot_6F0D91A0* SCheckedSlot_6F0D91A0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate
        && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                  GetConstant_6F0D91A0()))
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
