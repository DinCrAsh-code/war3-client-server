//============================================================================
//  0x6F085B50 - SWidgetAgentSlot::Assign.  See widgetagentslot.h.
//
//  Type-check the candidate, then swap: release the old value if it was the
//  last reference, and add a reference to the new one.  The check runs the
//  candidate's own vtable slot 7 (its agile type id) against
//  GetWidgetAgileTypeId's through AgileTypeIsDerivedFrom, and a candidate
//  that fails stores null rather than being rejected outright - which is
//  what makes this an assignment and not a guard.
//
//  The `m_value == candidate` early-out is the shipped code's: assigning
//  what is already held touches neither refcount.
//
//  Own translation unit: three real calls out of it.
//============================================================================
#include "widgetagentslot.h"
#include "agiletype.h"      // AgileTypeIsDerivedFrom
#include "agent.h"

SWidgetAgentSlot* SWidgetAgentSlot::Assign(CAgent* candidate)
{
    //  The whole shape is CAgentPtr::AssignChecked's (agent.cpp), leaf for
    //  leaf, with GetWidgetAgileTypeId's 'hgw+' where that one has the
    //  agile root - including testing `checked` and `candidate` separately
    //  before the add-ref even though the two are provably the same pointer
    //  by then.
    CAgent* checked;
    if (candidate &&
        AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetWidgetAgileTypeId()))
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
