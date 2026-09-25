//============================================================================
//  0x6F4A6420 - SRelationEndpointSlot::SaveTo.  See
//  agentrelationsavelinkref.cpp's own header comment for the real,
//  0x10-byte shape of one endpoint's embedded link node.
//
//  Own translation unit, deliberately: CAgentRelation::Save
//  (agentrelationsyncsave.cpp) calls this out of line, twice - so this
//  file only ever declares SaveLinkRef/WritePathRefEntry, never a caller
//  of its own alongside it.
//============================================================================
#include "agentrelation.h"
#include "cdatastorescratch.h"

void __fastcall WritePathRefEntry(CDataStoreScratch* store, const void* raw);
void __fastcall SaveLinkRef(CDataStoreScratch* store, const SRelationLink* link);

struct SRelationEndpointSlot
{
    SRelationLink* m_prev;      // +0x00
    SRelationLink* m_next;      // +0x04
    void*          m_owner;     // +0x08
    void*          m_endpoint;  // +0x0C

    void SaveTo(void* writer) const;
};

void SRelationEndpointSlot::SaveTo(void* writer) const
{
    CDataStoreScratch* store = (CDataStoreScratch*)writer;
    SaveLinkRef(store, m_prev);
    SaveLinkRef(store, m_next);
    WritePathRefEntry(store, m_owner);
    WritePathRefEntry(store, m_endpoint);
}
