//============================================================================
//  0x6F27C350 - SCheckedEventDataSlot's constructor (agenttypedslots.h).
//
//  Own translation unit, separate from its own Assign: the constructor
//  issues a genuine `call sub_6F271E90`.  Same __try/__finally as every
//  other constructor in the family - see checkedrectslot.cpp.
//
//  Reconstructed here rather than left redirected because a naked thunk
//  cannot stand in for a constructor at all: MSVC rejects
//  __declspec(naked) on one.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>   // AbnormalTermination(), for the constructor's __finally

SCheckedEventDataSlot::SCheckedEventDataSlot(CAgent* candidate)
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
