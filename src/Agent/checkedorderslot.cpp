//============================================================================
//  0x6F03B620 - SCheckedOrderSlot's constructor (agenttypedslots.h).
//
//  The same three statements every other one in the family is: zero the
//  value, Assign inside a `__try`, Release in a `__finally` guarded by
//  AbnormalTermination().  Its `retn 4` and its one call are all the
//  evidence there is - no call tree in `asm/` is rooted here - but the
//  family has thirteen other members and this one is reached exactly the
//  way JASS_Location reaches its own (`push 0` / `lea ecx,<local>` /
//  `call`, then the matching Assign and the inlined scope-exit release).
//
//  Own translation unit, separate from its own Assign, which it calls for
//  real.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>   // AbnormalTermination(), for the constructor's __finally

SCheckedOrderSlot::SCheckedOrderSlot(CAgent* candidate)
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
