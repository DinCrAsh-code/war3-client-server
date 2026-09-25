//============================================================================
//  0x6F27EFE0 - SCheckedTaskActionSlot's constructor (agenttypedslots.h).
//
//  The same three statements every other member of the family is: zero the
//  value, Assign inside a `__try`, Release in a `__finally` guarded by
//  AbnormalTermination().  Its one caller is CUnit::FinishCurrentOrder
//  (0x6F2964E0, unitorderfinish.cpp), which holds the slot as a scoped
//  local for the whole of its own body.
//
//  Own translation unit, separate from its own Assign, which it calls for
//  real.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>   // AbnormalTermination(), for the constructor's __finally

SCheckedTaskActionSlot::SCheckedTaskActionSlot(CAgent* candidate)
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
