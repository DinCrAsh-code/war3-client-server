//============================================================================
//  0x6F4A8CD0 - NIpse::CPoFgLis::Slot3.
//
//  ResetEventBinding(arg) through the base (Agent/agentrelation.h,
//  0x6F4A6070, reached the same way CPoLkLis::Slot3 reaches it), then
//  queries `arg`'s own +0x0C field (SPoFgLisMaskSource::QueryMask,
//  cpofglis_querymask.cpp - its own translation unit, deliberately, so
//  /Ob2 cannot inline it away here) and stores it into this class's own
//  +0x50 mask field, then sets flags bit 0x400 - same bit CPoLkLis::Slot3
//  sets.
//============================================================================
#include "cpofglis.h"
#include "agentrelation.h"

namespace NIpse {

void CPoFgLis::Slot3(SHandleRegistrationRequest* arg)
{
    ((CAgentRelation*)this)->ResetEventBinding(arg);
    unsigned int mask = (unsigned int)((SPoFgLisMaskSource*)arg)->QueryMask();
    ((CAgentRelation*)this)->m_flags |= 0x400;
    m_mask = mask;
}

}  // namespace NIpse
