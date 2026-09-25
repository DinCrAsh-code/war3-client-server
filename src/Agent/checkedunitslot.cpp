//============================================================================
//  0x6F038450 - SCheckedUnitSlot's constructor (agenttypedslots.h).
//
//  Own translation unit, separate from its own Assign in
//  unittypedassign.cpp: the constructor issues a genuine
//  `call sub_6F02A560`, so the two must not be in a position to be inlined
//  into each other.  (The item twin, 0x6F02A6A0/0x6F038890, sits at the
//  same two offsets in the same two modules and is split the same way.)
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>   // AbnormalTermination(), for the constructor's __finally

//----------------------------------------------------------------------------
//  0x6F038450.  The same __try/__finally the item twin has, for the same
//  reason: Assign's virtual GetAgileTypeId() call through `candidate` is
//  the one thing here that can raise, and the frame releases whatever
//  half-assigned value is left if it does.
//----------------------------------------------------------------------------
SCheckedUnitSlot::SCheckedUnitSlot(CAgent* candidate)
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
