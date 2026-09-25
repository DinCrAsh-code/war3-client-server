//============================================================================
//  0x6F4A5DF0 - SaveLinkRef.  See agentrelationsyncsave.cpp's own header
//  comment for the real, 0x10-byte shape of the SRelationLink neighbour
//  this reads (`link+8` is an owner back-pointer, not padding).
//
//  Own translation unit, deliberately: this function is small enough that
//  a naive layout would let MSVC inline it into its own callers
//  (SRelationEndpointSlot::SaveTo, SaveLinkRefIndirect) - the shipped
//  binary keeps it out of line (a real `call` at both of those call sites,
//  to this address), so this file only ever *declares* it is called, never
//  defines a caller alongside it.
//============================================================================
#include "agentrelation.h"
#include "cdatastorescratch.h"

void __fastcall WritePathRefEntry(CDataStoreScratch* store, const void* raw);

//----------------------------------------------------------------------------
//  The real, 0x10-byte shape of one endpoint's embedded link node - see
//  agentrelationsyncsave.cpp's own header comment.  Redeclared locally
//  here (own translation unit) the same way pathref_save.cpp/
//  pathref_load.cpp each keep their own local SPathRefReader/
//  SPathRefSaveEntry.
//----------------------------------------------------------------------------
struct SRelationEndpointSlot
{
    SRelationLink* m_prev;      // +0x00
    SRelationLink* m_next;      // +0x04
    void*          m_owner;     // +0x08
    void*          m_endpoint;  // +0x0C
};

//----------------------------------------------------------------------------
//  Write a reference to a doubly-linked-list neighbour: which
//  CAgentRelation owns it (through WritePathRefEntry, {-1,-1} for null) and
//  whether it is that owner's endpoint-B link (as opposed to A).  `link` is
//  read as an SRelationEndpointSlot only far enough to reach its own
//  +0x08 owner field - it may be any CAgentRelation's m_linkA or m_linkB,
//  not necessarily this object's.
//----------------------------------------------------------------------------
void __fastcall SaveLinkRef(CDataStoreScratch* store, const SRelationLink* link)
{
    const SRelationEndpointSlot* neighbour = (const SRelationEndpointSlot*)link;
    void* owner;
    if (link != 0)
        owner = neighbour->m_owner;
    else
        owner = 0;

    if (owner != 0 && (const char*)link == (const char*)owner + 0x34)
    {
        WritePathRefEntry(store, owner);
        store->WriteDword(1);
    }
    else
    {
        WritePathRefEntry(store, owner);
        store->WriteDword(0);
    }
}
