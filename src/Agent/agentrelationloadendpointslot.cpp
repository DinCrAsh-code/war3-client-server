//============================================================================
//  0x6F4A6460 - SRelationEndpointSlot::LoadFrom.  See
//  agentrelationsavelinkref.cpp's own header comment for the real,
//  0x10-byte shape of one endpoint's embedded link node.
//
//  Own translation unit, deliberately: CAgentRelation::Load
//  (agentrelationsyncload.cpp) calls this out of line, twice.
//============================================================================
#include "agentrelation.h"
#include "cdatastore.h"

SRelationLink* __fastcall LoadLinkRef(CDataStore* store);

//  0x6F4AF090 - read one path-ref entry back.  Out of scope; redirected -
//  see pathref_load.cpp, which declares the identical local wrapper for
//  the same reason.
struct SPathRefReader
{
    void* ReadEntry();
};

struct SRelationEndpointSlot
{
    SRelationLink* m_prev;      // +0x00
    SRelationLink* m_next;      // +0x04
    void*          m_owner;     // +0x08
    void*          m_endpoint;  // +0x0C

    void LoadFrom(void* reader);
};

void SRelationEndpointSlot::LoadFrom(void* reader)
{
    CDataStore* store = (CDataStore*)reader;

    SRelationLink* prev = LoadLinkRef(store);
    SRelationLink* next = LoadLinkRef(store);
    if (prev != 0)
        m_prev = prev;
    if (next != 0)
        m_next = next;

    m_owner    = ((SPathRefReader*)store)->ReadEntry();
    m_endpoint = ((SPathRefReader*)store)->ReadEntry();
}
