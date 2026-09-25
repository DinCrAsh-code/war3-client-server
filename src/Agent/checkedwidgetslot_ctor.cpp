//============================================================================
//  0x6F0382B0 - SCheckedWidgetSlot's constructor (agenttypedslots.h).
//
//  Own translation unit, separate from its own Assign in
//  widgettypedassign.cpp: the constructor issues a genuine
//  `call sub_6F029770`, so the two must not be in a position to be
//  inlined into each other - the same split checkedunitslot.cpp/
//  unittypedassign.cpp already use for the '+w3u' twin.
//
//  Surfaced by CUnit::CUnit() (unit_ctor.cpp), which constructs one of
//  these in place over its own m_pRef2C4 with a null candidate.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>   // AbnormalTermination(), for the constructor's __finally

//----------------------------------------------------------------------------
//  0x6F0382B0.  The same __try/__finally its '+w3u' twin
//  (SCheckedUnitSlot::SCheckedUnitSlot, checkedunitslot.cpp) has, for the
//  same reason: Assign's virtual GetAgileTypeId() call through
//  `candidate` is the one thing here that can raise, and the frame
//  releases whatever half-assigned value is left if it does.
//----------------------------------------------------------------------------
SCheckedWidgetSlot::SCheckedWidgetSlot(CAgent* candidate)
{
    m_value = 0;
    __try
    {
        Assign(candidate);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}
