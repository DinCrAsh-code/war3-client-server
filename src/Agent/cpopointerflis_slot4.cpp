//============================================================================
//  0x6F4A8210 - NIpse::CPoPoInterfLis::Slot4, vtable slot 4. If the +0x58
//  array's own m_count is non-zero, removes exactly `m_count` elements
//  starting at index 0 (`SInterfLisEntryArray::RemoveRange`,
//  cpopointerflis_array.cpp - functionally a full clear, since removing
//  every currently-live element from the front is what a `start=0,
//  count=m_count` call does), sets the base's own
//  `CAgentRelation::m_flags` bit 0x1000000, then always tail-calls the
//  base `CAgentRelation::Teardown` - not a bare `jmp` this time (unlike
//  CPoPoVelocityMod's/CPoLkLis's own Slot4, cpopovelocitymod_slot4.cpp/
//  cpolklis_slot4.cpp), since real array work precedes it here.
//============================================================================
#include "cpopointerflis.h"
#include "agentrelation.h"

namespace NIpse {

void CPoPoInterfLis::Slot4(int arg)
{
    if (m_array.m_count != 0)
        m_array.RemoveRange(0, m_array.m_count);

    ((CAgentRelation*)this)->m_flags |= 0x1000000;
    ((CAgentRelation*)this)->Teardown(arg);
}

}  // namespace NIpse
