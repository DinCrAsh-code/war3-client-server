//============================================================================
//  0x6F4A4EE0 - NIpse::CRlAgent::Save.
//
//  Reached through CAgentBaseAbs's own vtable slot 9 (+0x24), but that is
//  not who owns it: CPresenceTagged and CPrRelation both leave slot 9 a
//  no-op, CRlAgent is the first real override, and CAgentBaseAbs never
//  touches the slot again (CLAUDE.md's "lowest class in the chain whose own
//  address for that slot differs from its own base's"). Every field this
//  function touches (`m_relatedAgents` at +0x58, the four `SRelationLink`
//  heads at +0x78/+0x88/+0x98/+0xA8) is CRlAgent's own field per
//  Pathfinding/crlagent.h, which is the independent confirmation.
//
//  Calls straight into `CAgentRelation::Save` (0x6F4A6570,
//  agentrelationsyncsave.cpp) with the outer `this` untouched - which only
//  makes sense, and is the confirmation, that `CAgentRelation` in
//  agentrelation.h really is `NIpse::CPrRelation`, inherited at offset 0
//  (see agentbaseabs_related.h's own header comment). Then writes the
//  pending value at `this+0x50` (stamped by `NIpse::CRlAgent::Apply`,
//  0x6F4A4D70, agentbaseabs_slot3.cpp), the related-agent count, one entry
//  per related agent, and finally all four relation-kind lists through
//  `SaveLinkRefIndirect` (0x6F4A5E80, agentrelationsyncsave.cpp).
//============================================================================
#include "crlagent.h"
#include "agentbaseabs_related.h"
#include "cdatastorescratch.h"

void __fastcall WritePathRefEntry(CDataStoreScratch* store, const void* raw);
void __fastcall SaveLinkRefIndirect(CDataStoreScratch* store, void* handle);

namespace NIpse {

void CRlAgent::Save(void* store)
{
    ((CAgentRelation*)this)->Save(store);

    CDataStoreScratch* dataStore = (CDataStoreScratch*)store;
    WritePathRefEntry(dataStore, *(void**)((char*)this + 0x50));

    dataStore->WriteDword(*(unsigned int*)((char*)this + 0x74));

    unsigned int i = 0;
    if (i < *(unsigned int*)((char*)this + 0x74))
    {
        do
        {
            CAgentRelation** data = *(CAgentRelation***)((char*)this + 0x64);
            WritePathRefEntry(dataStore, data[i]);
            ++i;
        } while (i < *(unsigned int*)((char*)this + 0x74));
    }

    SaveLinkRefIndirect(dataStore, (char*)this + 0x78);
    SaveLinkRefIndirect(dataStore, (char*)this + 0x88);
    SaveLinkRefIndirect(dataStore, (char*)this + 0x98);
    SaveLinkRefIndirect(dataStore, (char*)this + 0xA8);
}

}  // namespace NIpse
