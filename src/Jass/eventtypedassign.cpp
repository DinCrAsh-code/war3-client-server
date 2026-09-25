//============================================================================
//  0x6F3B7560 - SCheckedEventSlot::Assign.  See agenttypedslots.h for the
//  shape and itemtypedassign.cpp for why it is spelled the way it is.
//
//  Checks the candidate against the JASS `event` object's '+evt' agile type
//  id.  Its own translation unit, like every other member of the family: its
//  constructor (0x6F3BA320) issues a genuine `call` to it.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

//  0x6F4327D0 - '+evt' (jassagiletypeids.cpp).
unsigned int GetEventAgileTypeId();

SCheckedEventSlot* SCheckedEventSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                            GetEventAgileTypeId()))
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
