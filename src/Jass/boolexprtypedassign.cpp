//============================================================================
//  0x6F3B55C0 - SCheckedBoolExprSlot::Assign.  See agenttypedslots.h for the shape and
//  itemtypedassign.cpp for why it is spelled the way it is; neither is
//  repeated here.
//
//  Checks the candidate against the JASS `boolexpr` object's own 'bExp' agile
//  type id.  Its own translation unit, like every other member of the
//  family: its constructor (0x6F3B8C10) issues a genuine `call` to it.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

//  0x6F3B15D0 - 'bExp' (jassagiletypeids.cpp).
unsigned int GetBoolExprAgileTypeId();

SCheckedBoolExprSlot* SCheckedBoolExprSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                            GetBoolExprAgileTypeId()))
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
