//============================================================================
//  0x6F0EC480 - SCheckedSpellAbilitySlot's constructor (agenttypedslots.h).
//
//  Own translation unit, separate from its own Assign: the constructor
//  issues a genuine `call sub_6F0E0AC0`.  Same __try/__finally as every
//  other constructor in the family - see checkedrectslot.cpp.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>   // AbnormalTermination(), for the constructor's __finally

SCheckedSpellAbilitySlot::SCheckedSpellAbilitySlot(CAgent* candidate)
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
