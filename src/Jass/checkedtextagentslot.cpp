//============================================================================
//  0x6F3A1D00 - SCheckedTextAgentSlot's constructor (agenttypedslots.h).
//
//  The same three-statement shape (zero the value, Assign inside a __try,
//  Release in a __finally guarded by AbnormalTermination()) every sibling
//  constructor in this family has - see checkedtriggerslot.cpp and
//  itemtypedassign.cpp for why it is spelled this way and why the score is
//  capped by the SEH frame rather than by anything in the logic itself.
//
//  ExecuteFunc's own call site (jassruntimeexecutefunc.cpp) always passes a
//  null candidate here: this is a plain "give me a zeroed, type-checked
//  scratch slot" default-construct, immediately followed by a real
//  MakeTextAgent() call on the same local that actually fills it in.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>   // AbnormalTermination(), for the constructor's __finally

SCheckedTextAgentSlot::SCheckedTextAgentSlot(CAgent* candidate)
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
