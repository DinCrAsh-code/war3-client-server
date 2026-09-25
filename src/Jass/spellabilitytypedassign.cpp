//============================================================================
//  0x6F0E0AC0 - SCheckedSpellAbilitySlot::Assign.  See agenttypedslots.h for
//  the shape and itemtypedassign.cpp for why it is spelled the way it is.
//
//  Checks the candidate against the 'AAsm' ability tag every `GetSpell*`
//  native's object has to carry.  Own translation unit: its constructor
//  (0x6F0EC480) issues a genuine `call` to it.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

//  0x6F028B10 - 'AAsm' (jassagiletypeids.cpp).
unsigned int GetSpellAbilityAgileTypeId();

SCheckedSpellAbilitySlot* SCheckedSpellAbilitySlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                            GetSpellAbilityAgileTypeId()))
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
