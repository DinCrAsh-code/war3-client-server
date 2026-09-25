//============================================================================
//  0x6F0955C0 - SWidgetAgentSlot's constructor: start empty, then run the
//  ordinary type-checked assignment.
//
//  Its own translation unit because that assignment is a real `call`
//  (widgetagentslot.cpp).  Same unreproducible __except_handler4-shaped SEH
//  frame as every other constructor in this family
//  (docs/msvc-vc8-idioms.md): the assignment can throw, and the half-built
//  slot has to be unwound.
//============================================================================
#include "widgetagentslot.h"
#include <excpt.h>   // AbnormalTermination(), for the __finally below

SWidgetAgentSlot::SWidgetAgentSlot(CAgent* candidate)
{
    //  The same __try/__finally CAgentPtr's own converting constructor has
    //  (agent.cpp) and for the same reason: Assign's virtual call is the
    //  one thing here the compiler cannot prove will not raise a structured
    //  exception, and a half-assigned slot has to be released if it does.
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
