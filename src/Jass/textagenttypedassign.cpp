//============================================================================
//  0x6F39FF70 - SCheckedTextAgentSlot::Assign.  See agenttypedslots.h for
//  the shape and Item/itemtypedassign.cpp for why it is spelled the way it
//  is; neither is repeated here.
//
//  Checks the candidate against the same '+tex' floating-text-agent tag
//  GameUI/gameuiagenttypetag.cpp's GetTextAgentTypeId() already names - not
//  a JASS-specific type of its own.  Its own translation unit, like every
//  other member of the family: SCheckedTextAgentSlot::MakeTextAgent
//  (jasstextagentmake.cpp) issues a genuine `call` to it, and so does the
//  constructor (checkedtextagentslot.cpp).
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

//  0x6F443C60 - '+tex' (GameUI/gameuiagenttypetag.cpp).
unsigned int GetTextAgentTypeId();

SCheckedTextAgentSlot* SCheckedTextAgentSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                            GetTextAgentTypeId()))
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
