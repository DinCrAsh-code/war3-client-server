//============================================================================
//  SWidgetAgentSlot - the type-checked, reference-counted CAgent* CWidget's
//  vtable slot 68 holds a resolved widget-side object in.
//
//  The same general shape as CAgentPtr (agent.h) and SCheckedItemSlot
//  (itemtypedassign.h): assignment runs the candidate's own agile type id
//  through AgileTypeIsDerivedFrom against one fixed type - here
//  GetWidgetAgileTypeId's 'hgw+' - and stores null when the check fails,
//  releasing whatever was held either way.  A different instantiation from
//  both of those: its own two addresses, its own leaf type.
//============================================================================
#ifndef WIDGETAGENTSLOT_H
#define WIDGETAGENTSLOT_H

#include "agent.h"
#include "storm.h"      // TSGrowableArray

struct SWidgetAgentSlot
{
    //  0x6F0955C0 - construct empty and assign in one step.
    SWidgetAgentSlot(CAgent* candidate);

    //  0x6F085B50 - the type-checked assignment.
    SWidgetAgentSlot* Assign(CAgent* candidate);

    //  Inline, the same way CAgentPtr::Release is (agent.h): every one of
    //  these instantiations has its release inlined at its call sites
    //  rather than reached out of line.  m_value is copied into a local
    //  first, the same shape SCheckedUnitSlot::Release (agenttypedslots.h)
    //  uses - a direct `if (m_value)` re-reads the field a second time at
    //  every call site that inlines this, costing the array growth code's
    //  own shrink loop an extra reload and a `cmp` in place of the shipped
    //  `add`'s own flags.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;    // +0x00
};

//  0x6F2C6140 - agilewidgettype.cpp.
unsigned int GetWidgetAgileTypeId();

#endif
