//============================================================================
//  0x6F271E90 - SCheckedEventDataSlot::Assign.  See agenttypedslots.h for the
//  shape and itemtypedassign.cpp for why it is spelled the way it is.
//
//  Checks the candidate against CScriptEventData's own 'wscd' agile type id
//  (misc_rawcode_getters.cpp).  Own translation unit: its constructor
//  (0x6F27C350) issues a genuine `call` to it.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

//  0x6F431EA0 - 'wscd' (misc_rawcode_getters.cpp).
unsigned int GetScriptEventDataAgileTypeId();

SCheckedEventDataSlot* SCheckedEventDataSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                            GetScriptEventDataAgileTypeId()))
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
