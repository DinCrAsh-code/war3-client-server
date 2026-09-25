//============================================================================
//  0x6F2C86C0 - SCheckedSelectableSlot::Assign.  See agenttypedslots.h.
//
//  Checks the candidate against CSelectable's own '+w3s' agile type id.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

unsigned int GetTypeTag_6F2C73C0();   //  0x6F2C73C0 - '+w3s', CSelectable's agile type id.

SCheckedSelectableSlot* SCheckedSelectableSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetTypeTag_6F2C73C0()))
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
