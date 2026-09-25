//============================================================================
//  0x6F010420 - SCheckedDestructableSlot::Assign.  See agenttypedslots.h.
//
//  Checks the candidate against CDestructable's own '+w3d' agile type id.
//  The one thing that makes this body different from its fourteen siblings
//  is that the required id arrives from a *call* rather than as an
//  immediate, and that the call happens before the candidate's own virtual
//  slot 7 - MSVC evaluates AgileTypeIsDerivedFrom's arguments right to
//  left, so writing the getter as the second argument is what puts it
//  first in the stream.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

unsigned int GetDestructableTypeFourCC();  //  0x6F266160 - '+w3d'.

SCheckedDestructableSlot*
SCheckedDestructableSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate
        && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                  GetDestructableTypeFourCC()))
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
