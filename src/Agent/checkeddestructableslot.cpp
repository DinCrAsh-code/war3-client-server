//============================================================================
//  0x6F012CF0 - SCheckedDestructableSlot's constructor (agenttypedslots.h).
//
//  Own translation unit, separate from its own Assign in
//  destructabletypedassign.cpp, for the reason checkedunitslot.cpp gives:
//  the constructor issues a genuine `call sub_6F010420`, so the two must
//  not be in a position to be inlined into each other.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>   // AbnormalTermination(), for the constructor's __finally

//----------------------------------------------------------------------------
//  0x6F012CF0.  The class's converting constructor, spelled as an ordinary
//  member for the reason agenttypedslots.h gives.  The same __try/__finally
//  every member of this family has:
//  Assign's virtual GetAgileTypeId() call through `candidate` is the one
//  thing here that can raise, and the frame releases whatever half-assigned
//  value is left if it does.
//----------------------------------------------------------------------------
SCheckedDestructableSlot* SCheckedDestructableSlot::Construct(CAgent* candidate)
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

    return this;
}
