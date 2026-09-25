//============================================================================
//  0x6F029770 - SCheckedWidgetSlot::Assign.  See agenttypedslots.h.
//
//  Checks the candidate against CWidget's own '+w3w' agile type id.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

unsigned int GetTypeTag_6F2AB090();   //  0x6F2AB090 - '+w3w', CWidget's agile type id.

SCheckedWidgetSlot* SCheckedWidgetSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetTypeTag_6F2AB090()))
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
