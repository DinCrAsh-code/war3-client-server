//============================================================================
//  0x6F4AA230 - NIpse::CPoLkLis::Slot3, vtable slot 3.
//
//  Calls straight into CAgentRelation's own ResetEventBinding
//  (Agent/agentrelation.h, 0x6F4A6070) exactly the way
//  Agent/agentbaseabs_slot3.cpp's own `CRlAgent::Apply` already reaches it
//  through the same explicit `(CAgentRelation*)this` cast - this class *is*
//  a CAgentRelation at offset 0, per that file's own header comment - then
//  sets flags bit 0x400 (Agent/agentrelation.h's `m_flags`, +0x4C). No
//  existing `kRel*` constant in that header names this bit; it is not
//  tested again anywhere in this class's own new slots.
//============================================================================
#include "cpolklis.h"
#include "agentrelation.h"

namespace NIpse {

void CPoLkLis::Slot3(SHandleRegistrationRequest* arg)
{
    ((CAgentRelation*)this)->ResetEventBinding(arg);
    ((CAgentRelation*)this)->m_flags |= 0x400;
}

}  // namespace NIpse
