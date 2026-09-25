//============================================================================
//  0x6F3B8FA0 - SCheckedLocationSlot's constructor (agenttypedslots.h).
//
//  Own translation unit, separate from its own Assign in
//  locationtypedassign.cpp: the constructor issues a genuine
//  `call sub_6F3B62E0`, so the two must not be in a position to be inlined
//  into each other - the same split checkedunitslot.cpp/unittypedassign.cpp
//  and checkedqumslot.cpp already have for their own pairs.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>   // AbnormalTermination(), for the constructor's __finally

//----------------------------------------------------------------------------
//  0x6F3B8FA0.  The same __try/__finally the unit and '+qum' twins have, for
//  the same reason: Assign's virtual GetAgileTypeId() call through
//  `candidate` is the one thing here that can raise, and the frame releases
//  whatever half-assigned value is left if it does.
//----------------------------------------------------------------------------
SCheckedLocationSlot::SCheckedLocationSlot(CAgent* candidate)
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
