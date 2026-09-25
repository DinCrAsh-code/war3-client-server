//============================================================================
//  0x6F3B62E0 - SCheckedLocationSlot::Assign.  See agenttypedslots.h.
//
//  Checks the candidate against the JASS `location` object's own '+loc'
//  agile type id.  The eighth instantiation of a body that is byte-for-byte
//  the same as the other seven with one immediate changed; nothing about it
//  is new, which is the point of recording it here rather than re-deriving
//  the shape from the disassembly.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

//  0x6F4326D0 - '+loc' (misc_rawcode_getters.cpp).
unsigned int GetLocationAgileTypeId();

SCheckedLocationSlot* SCheckedLocationSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                            GetLocationAgileTypeId()))
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
