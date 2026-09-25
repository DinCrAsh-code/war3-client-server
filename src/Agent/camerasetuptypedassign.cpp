//============================================================================
//  0x6F3B7300 - SCheckedCameraSetupSlot::Assign.  See agenttypedslots.h.
//
//  Checks the candidate against the JASS `camerasetup` object's own '+cst'
//  agile type id (0x6F3DA570, misc_rawcode_getters.cpp).  The sixteenth
//  instantiation of a body that is byte-for-byte the same as the others
//  with one immediate changed.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

//  0x6F3DA570 - '+cst' (misc_rawcode_getters.cpp).
unsigned int GetCameraSetupAgileTypeId();

SCheckedCameraSetupSlot* SCheckedCameraSetupSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                            GetCameraSetupAgileTypeId()))
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
