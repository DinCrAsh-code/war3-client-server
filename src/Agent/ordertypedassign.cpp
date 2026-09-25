//============================================================================
//  0x6F02B360 - SCheckedOrderSlot::Assign.  See agenttypedslots.h.
//
//  Checks the candidate against the unit-order object's own '+ord' agile
//  type id (0x6F2712B0, misc_rawcode_getters.cpp).  The seventeenth
//  instantiation of a body that is byte-for-byte the same as the others
//  with one immediate changed - and here that is literal: its 0x5C bytes
//  match SCheckedCameraSetupSlot::Assign's at 0x6F3B7300 instruction for
//  instruction, and the one `call` in it resolves to GetOrderAgileTypeId
//  and to nothing else, which is what fixes the type without a dump.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

//  0x6F2712B0 - '+ord' (misc_rawcode_getters.cpp).
unsigned int GetOrderAgileTypeId();

SCheckedOrderSlot* SCheckedOrderSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                            GetOrderAgileTypeId()))
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
