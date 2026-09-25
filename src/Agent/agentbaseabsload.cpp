//============================================================================
//  0x6F4A5AD0 - NIpse::CRlAgent::Load.  See crlagent.h's own comment for
//  the full field order; this is the Load-side mirror of
//  Agent/agentbaseabssave.cpp's own `NIpse::CRlAgent::Save`
//  (0x6F4A4EE0, a *different* parallel session's own batch of this same
//  dump), which this file's own comments cross-reference for the
//  +0x50/table/list-head field order.
//
//  0x6F4A45B0 (the fix-up call at the very end of Load) is out of this
//  session's own batch, owned by a parallel session - declared locally the
//  same way agentbaseabsrelatedcascade.cpp's own cross-batch calls are.
//
//  Own translation unit, deliberately, w.r.t. LoadRelationLink
//  (0x6F4A5E90, agentbaseabsloadrelationlink.cpp): the shipped body calls
//  it out of line, four times.
//============================================================================
#include "crlagent.h"
#include "agentrelation.h"
#include "agentbaseabs_related.h"
#include "cdatastore.h"

void __fastcall LoadRelationLink(CDataStore* store, void* slot);

//  0x6F4AF090 - read one path-ref entry back.  Out of scope; redirected -
//  see pathref_load.cpp / agentrelationsyncload.cpp, which each declare
//  the identical local wrapper for the same reason.
struct SPathRefReader
{
    void* ReadEntry();
};

namespace NIpse {

void CRlAgent::Load(void* reader)
{
    CDataStore* store = (CDataStore*)reader;

    ((::CAgentRelation*)this)->Load(reader);

    void* pending = ((SPathRefReader*)store)->ReadEntry();
    *(void**)((char*)this + 0x50) = pending;

    unsigned int count = 0;
    store->ReadDword(&count);

    if (count > 0)
    {
        CRelatedAgentsTable* table = (CRelatedAgentsTable*)((char*)this + 0x58);
        for (unsigned int i = 0; i < count; ++i)
        {
            void* item = ((SPathRefReader*)store)->ReadEntry();
            table->GrowInsert(&item, 1);
        }
    }

    LoadRelationLink(store, (char*)this + 0x78);
    LoadRelationLink(store, (char*)this + 0x88);
    LoadRelationLink(store, (char*)this + 0x98);
    LoadRelationLink(store, (char*)this + 0xA8);

    if (pending != 0)
        ((CAgentBaseAbsSlot3Result*)((char*)pending + 0x50))->FixUp_6F4A45B0(this);
}

}  // namespace NIpse
