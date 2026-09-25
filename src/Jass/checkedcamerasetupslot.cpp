//============================================================================
//  0x6F3B9C00 - SCheckedCameraSetupSlot's constructor (agenttypedslots.h).
//
//  The thirteenth of the family, and the same three statements every other
//  one is: zero the value, Assign inside a `__try`, Release in a
//  `__finally` guarded by AbnormalTermination().
//
//  Own translation unit, separate from its own Assign: the constructor
//  issues a genuine `call sub_6F3B7300`, so the two must not be in a
//  position to be inlined into each other.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>   // AbnormalTermination(), for the constructor's __finally

SCheckedCameraSetupSlot::SCheckedCameraSetupSlot(CAgent* candidate)
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
