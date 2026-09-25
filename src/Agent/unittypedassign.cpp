//============================================================================
//  0x6F02A560 - SCheckedUnitSlot::Assign.  See agenttypedslots.h.
//
//  Checks the candidate against CUnit's own '+w3u' agile type id.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

unsigned int GetUnitTypeFourCC();      //  0x6F26C1C0 - '+w3u' (misc_rawcode_getters.cpp).

SCheckedUnitSlot* SCheckedUnitSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetUnitTypeFourCC()))
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
