//============================================================================
//  0x6F0388F0 - SCheckedOrderTargetSlot's constructor (agenttypedslots.h).
//
//  The same three statements every other member of the family is: zero the
//  value, Assign inside a `__try`, Release in a `__finally` guarded by
//  AbnormalTermination().  Its one caller is MakeOrderTargetAgent
//  (0x6F294D40, unitordertargetmake.cpp), which holds the slot as a scoped
//  local exactly the way the `Location` native holds its own.
//
//  Own translation unit, separate from its own Assign, which it calls for
//  real.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>   // AbnormalTermination(), for the constructor's __finally

SCheckedOrderTargetSlot::SCheckedOrderTargetSlot(CAgent* candidate)
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
