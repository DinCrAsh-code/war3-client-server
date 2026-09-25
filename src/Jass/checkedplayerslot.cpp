//============================================================================
//  0x6F388950 - SCheckedPlayerSlot's constructor (agenttypedslots.h).
//
//  Own translation unit, separate from its own Assign: the constructor
//  issues a genuine `call sub_6F387730`, so the two must not be in a position
//  to be inlined into each other.
//
//  The same __try/__finally every other constructor in this family has, for
//  the same reason: Assign's virtual GetAgileTypeId() call through
//  `candidate` is the one thing here that can raise, and the frame releases
//  whatever half-assigned value is left if it does.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>   // AbnormalTermination(), for the constructor's __finally

SCheckedPlayerSlot::SCheckedPlayerSlot(CAgent* candidate)
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
