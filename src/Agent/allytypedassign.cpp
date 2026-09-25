//============================================================================
//  0x6F40CC60 - SCheckedAllySlot::Assign.  See agenttypedslots.h.
//
//  Checks the candidate against the 'ally' type id.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

unsigned int GetAllyFourCC();         //  0x6F3D9E30 - 'ally' (misc_rawcode_getters.cpp).

SCheckedAllySlot* SCheckedAllySlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAllyFourCC()))
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
