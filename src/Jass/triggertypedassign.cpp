//============================================================================
//  0x6F3B6580 - SCheckedTriggerSlot::Assign.  See agenttypedslots.h for the shape and
//  itemtypedassign.cpp for why it is spelled the way it is; neither is
//  repeated here.
//
//  Checks the candidate against the JASS `trigger` object's own '+trg' agile
//  type id.  Its own translation unit, like every other member of the
//  family: its constructor (0x6F3B91E0) issues a genuine `call` to it.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

//  0x6F443D50 - '+trg' (jassagiletypeids.cpp).
unsigned int GetTriggerAgileTypeId();

SCheckedTriggerSlot* SCheckedTriggerSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                            GetTriggerAgileTypeId()))
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
