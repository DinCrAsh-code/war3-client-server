//============================================================================
//  0x6F3B6260 - SCheckedRegionSlot::Assign.  See agenttypedslots.h for the shape and
//  itemtypedassign.cpp for why it is spelled the way it is; neither is
//  repeated here.
//
//  Checks the candidate against the JASS `region` object's own '+agr' agile
//  type id.  Its own translation unit, like every other member of the
//  family: its constructor (0x6F3B8F40) issues a genuine `call` to it.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

//  '+agr', loaded as an immediate rather than fetched from a getter: this
//  copy of the body has no `call` before AgileTypeIsDerivedFrom, which is
//  why it is forty instructions where its eight siblings are forty-two.
const unsigned int kRegionAgileTypeId = 0x2B616772;

SCheckedRegionSlot* SCheckedRegionSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                            kRegionAgileTypeId))
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
