//============================================================================
//  0x6F2758B0 - SCheckedAseiSlot::Assign.  cunit_agent7 batch (CUnit closure
//  BFS).  See agenttypedslots.h and itemtypedassign.cpp.
//
//  The required type id is GetTypeTag_6F028840 (already reconstructed and
//  EXACT, Misc/trivialaccessors_02_03.cpp) - its one stack-less
//  `__fastcall(void* self)` parameter is `this`, unread inside, matching
//  this call site exactly: ecx is still `this` (untouched since function
//  entry) at the point of the call.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

extern unsigned int __fastcall GetTypeTag_6F028840(void* self);

SCheckedAseiSlot* SCheckedAseiSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetTypeTag_6F028840(this)))
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
