//============================================================================
//  0x6F4A5710 - NIpse::CRlAgent::SubmitRelatedAgentEvent.
//
//  CORRECTION (2026-09-07, vtable-derived): this used to be scoped under a
//  `CAgentBaseAbsRelated` struct - it is not a vtable slot at all (reached
//  only through DispatchOrder's switch, Agent/agentbaseabsorderdispatch.cpp)
//  and every field it touches (`m_relatedAgents` at +0x58) belongs to
//  `NIpse::CRlAgent` (Pathfinding/crlagent.h), which is where it's declared
//  now.
//
//  Copies a caller-supplied `SWidgetAgentQuery` (Widget/widgetagentquery.h -
//  the same 11-dword shape agentrelation.h's own `SAgentTickEventCtx`
//  comment already flags as reused elsewhere in this module) onto the
//  stack, overwrites its `m_reserved24`/`m_reserved28` pair unless this
//  agent's own `+0x14` sign bit is set or the copy's own `m_reserved1C` low
//  two bits are set (in which case the copy's `-1`/`-1` defaults are
//  replaced with a `0xFFFFFFFE` sentinel instead - not `-1`; transcribed
//  exactly, the shipped code never explains the different sentinel value),
//  or, when `table` is given, by reading a `{id, extra}` pair out of a
//  completely different, 12-byte-element array `table` itself owns
//  (indexed by this agent's own related-agent *count* - not a self-
//  reference to `m_relatedAgents`, the shipped code really does reuse
//  `+0x74` both ways here). Submits it via `SubmitWidgetAgentQuery`
//  (0x6F48AB80, Widget/widgetagentsubmit.cpp), marks the result and stamps
//  its own related-agent count into it, then inserts the made object as a
//  new related agent (`CRelatedAgentsTable::GrowInsert`,
//  agentbaseabsrelatedtable.cpp).
//============================================================================
#include "crlagent.h"
#include "agentbaseabs_related.h"
#include "widgetagentquery.h"

//  `table`'s own `+0x0C` - the base of a 12-byte-stride record array this
//  function indexes by this agent's own related-agent count. Nothing else
//  in this call tree reaches `table`, so only this one field is named.
struct SAgentEventLookupTable
{
    char                m_reserved00[0x0C];
    const unsigned int* m_data;   // +0x0C
};

namespace NIpse {

void CRlAgent::SubmitRelatedAgentEvent(const void* ctx,
                                       const void* table)
{
    SWidgetAgentQuery local = *(const SWidgetAgentQuery*)ctx;

    if ((*(unsigned int*)((char*)this + 0x14) & 0x80000000u) != 0 ||
        (local.m_reserved1C & 3u) != 0)
    {
        local.m_reserved24 = 0xFFFFFFFEu;
    }
    else if (table != 0)
    {
        unsigned int index = *(unsigned int*)((char*)this + 0x74);
        const unsigned int* rec =
            ((const SAgentEventLookupTable*)table)->m_data + index * 3;
        unsigned int id = rec[0];
        unsigned int extra = rec[1];
        local.m_reserved24 = id;
        local.m_reserved28 = extra;
    }

    void* made = SubmitWidgetAgentQuery(&local, 1, 1);
    *(unsigned int*)((char*)made + 0x4C) |= 0x80000000u;
    *(unsigned int*)((char*)made + 0x48) = *(unsigned int*)((char*)this + 0x74);

    void* madeItem = made;
    ((CRelatedAgentsTable*)((char*)this + 0x58))->GrowInsert(&madeItem, 1);
}

}  // namespace NIpse
