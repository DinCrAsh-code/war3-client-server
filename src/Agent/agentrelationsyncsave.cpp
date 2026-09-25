//============================================================================
//  0x6F4A6570 - CAgentRelation::Save.  See agentrelation.h's own comment
//  for the full field order.
//
//  Own translation unit, deliberately: the shipped body calls
//  SRelationEndpointSlot::SaveTo (agentrelationsaveendpointslot.cpp) and
//  WriteSyncRegistrationRecord (agentrelationwritesyncrecord.cpp)
//  out of line, twice each - so this file only ever declares them, never
//  defines them alongside this one, the same reasoning
//  agentrelationsavelinkref.cpp's own header comment gives.
//============================================================================
#include "agentrelation.h"
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
void __fastcall WriteSyncRegistrationRecord(CDataStoreScratch* store, const SSyncRegistration* reg);

struct SRelationEndpointSlot
{
    SRelationLink* m_prev;
    SRelationLink* m_next;
    void*          m_owner;
    void*          m_endpoint;

    void SaveTo(void* writer) const;
};

void CAgentRelation::Save(void* writer)
{
    unsigned int flags = m_flags;
    unsigned int eventId = m_eventId;
    CDataStoreScratch* store = (CDataStoreScratch*)writer;
    CDataStoreScratch* chain = WriteCFloat(store, &m_field44);
    chain = chain->WriteDword(eventId);
    chain = chain->WriteDwordAlt2(flags);

    ((const SRelationEndpointSlot*)&m_linkA)->SaveTo(chain);
    ((const SRelationEndpointSlot*)&m_linkB)->SaveTo(chain);

    WriteSyncRegistrationRecord(chain, m_registration);
    WriteSyncRegistrationRecord(chain, m_channelReg);
}
