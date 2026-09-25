//============================================================================
//  0x6F4A6C10 - CAgentRelation::Load.  See agentrelation.h's own comment
//  for the full field order.  Load does not blit the two saved
//  registrations back by value: a deserialized SSyncRegistration has to go
//  back through its channel's own bookkeeping (CSyncChannel::MakeRequest,
//  syncchannelrequest.cpp), so this re-derives this relation's own channel
//  (CTimeSync::GetChannel) and re-issues both registrations through it
//  instead (RebuildSyncRegistration, agentrelationrebuildsyncrecord.cpp).
//
//  Own translation unit, deliberately: the shipped body calls
//  SRelationEndpointSlot::LoadFrom (agentrelationloadendpointslot.cpp) and
//  RebuildSyncRegistration out of line, so this file only ever declares
//  them - same reasoning as agentrelationsyncsave.cpp's own header
//  comment, mirrored on the read side.
//============================================================================
#include "agentrelation.h"
#include "cdatastore.h"

CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);
SSyncRegistration* __fastcall RebuildSyncRegistration(CDataStore* store, CSyncChannel* channel, void* owner);

struct SRelationEndpointSlot
{
    SRelationLink* m_prev;
    SRelationLink* m_next;
    void*          m_owner;
    void*          m_endpoint;

    void LoadFrom(void* reader);
};

void CAgentRelation::Load(void* reader)
{
    CDataStore* store = (CDataStore*)reader;

    CDataStore* chain = ReadCFloat(store, &m_field44);
    chain = chain->ReadDword(&m_eventId);
    chain = chain->ReadDwordAlt3(&m_flags);

    ((SRelationEndpointSlot*)&m_linkA)->LoadFrom(chain);
    ((SRelationEndpointSlot*)&m_linkB)->LoadFrom(chain);

    CSyncChannel* channel = (CSyncChannel*)g_pTimeSync->GetChannel((const CHandleObject*)this);

    m_registration = RebuildSyncRegistration(chain, channel, this);
    m_channelReg   = RebuildSyncRegistration(chain, channel, this);
}
