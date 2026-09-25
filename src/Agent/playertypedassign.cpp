//============================================================================
//  0x6F387730 - the '+ply' member of the type-checked agent-slot family, and
//  the seventh copy of that body in this image.  See agenttypedslots.h for
//  the shape, itemtypedassign.cpp for why it is spelled the way it is;
//  neither is repeated here.
//
//  What names it is the required type: the getter this copy calls is
//  0x6F40AFA0, already reconstructed in trivialaccessors_04.cpp as the
//  constant 0x2B706C79 - "+ply" read most-significant byte first.  The unit
//  factory (0x6F29F990, jasscreateunit.h) is what reaches it, parking the
//  owning player in a slot on the unit it is building.
//
//  The one thing worth saying that the family header does not: this copy's
//  type-tag getter takes its receiver in ecx, and the shipped code calls it
//  with the slot's own `this` still there from the prologue - so it is
//  spelled `GetTypeTag_6F40AFA0(this)` rather than with a null, and the
//  argument costs nothing because ecx already holds it.
//
//  Its own translation unit, like every other member of the family.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

//  0x6F40AFA0 - trivialaccessors_04.cpp.  Re-declared, not redefined.
unsigned int __fastcall GetTypeTag_6F40AFA0(void* self);

SCheckedPlayerSlot* SCheckedPlayerSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                            GetTypeTag_6F40AFA0(this)))
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
