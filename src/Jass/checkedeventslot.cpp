//============================================================================
//  0x6F3BA320 - SCheckedEventSlot's constructor (agenttypedslots.h).
//
//  Own translation unit, separate from its own Assign: the constructor
//  issues a genuine `call sub_6F3B7560`, so the two must not be in a
//  position to be inlined into each other.  Same __try/__finally as every
//  other constructor in the family - see checkedrectslot.cpp.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>   // AbnormalTermination(), for the constructor's __finally

SCheckedEventSlot::SCheckedEventSlot(CAgent* candidate)
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
