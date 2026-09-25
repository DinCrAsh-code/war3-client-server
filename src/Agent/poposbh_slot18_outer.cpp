//============================================================================
//  0x6F4AB460 - NIpse::CPoPosBh::Slot18, vtable slot 18. See poposbh.h's
//  own header comment. Own translation unit, separate from
//  CPoPosBh::Slot18Helper (poposbh_slot18.cpp) - see that file's own
//  header comment for why.
//============================================================================
#include "poposbh.h"
#include "agentrelation.h"

namespace NIpse {

void CPoPosBh::Slot18(void* handle)
{
    if (handle == m_syncReg)
    {
        Slot18Helper();
        return;
    }

    ((CAgentRelation*)this)->OnRegistrationRetired(handle);
}

}  // namespace NIpse
