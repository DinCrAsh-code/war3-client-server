//============================================================================
//  0x6F02A720 - SCheckedOrderTargetSlot::Assign.  See agenttypedslots.h.
//
//  Checks the candidate against COrderTarget's own 'ordt' agile type id
//  (0x6F271430, misc_rawcode_getters.cpp).  Like the '+w3d' one and unlike
//  the sixteen that bake an immediate in, the required id arrives from a
//  *call*, and that call happens before the candidate's own virtual slot 7
//  because MSVC evaluates AgileTypeIsDerivedFrom's arguments right to left.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

unsigned int GetOrderTargetAgileTypeId();   //  0x6F271430 - 'ordt'.

SCheckedOrderTargetSlot*
SCheckedOrderTargetSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate
        && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                  GetOrderTargetAgileTypeId()))
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
