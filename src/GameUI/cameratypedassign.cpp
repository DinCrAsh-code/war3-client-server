//============================================================================
//  0x6F2F49E0 - SCheckedCameraSlot::Assign, the sixteenth copy of the
//  type-checked agent-slot body in this image.  See agenttypedslots.h for
//  the shape and itemtypedassign.cpp for why it is spelled the way it is;
//  neither is repeated here.
//
//  The required type is '+cam': the getter this copy calls is 0x6F301E10,
//  already reconstructed in trivialaccessors_02_03.cpp as the constant
//  0x2B63616D.  It is called as GetTypeTag_6F301E10(this) rather than with
//  a null for the same reason playertypedassign.cpp gives - the shipped
//  code calls it with the slot's own `this` still in ecx from the
//  prologue, and the argument costs nothing.
//
//  Its own translation unit, like every other member of the family: its
//  constructor (0x6F2F77A0) issues a genuine `call` to it.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

//  0x6F301E10 - trivialaccessors_02_03.cpp.  Re-declared, not redefined.
unsigned int __fastcall GetTypeTag_6F301E10(void* self);

SCheckedCameraSlot* SCheckedCameraSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                            GetTypeTag_6F301E10(this)))
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
