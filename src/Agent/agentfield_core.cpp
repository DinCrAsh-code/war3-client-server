//============================================================================
//  0x6F478820 - 0x6F478943 - four of CAgentField's vtable slots that
//  shipped together as one module: GetAgent (5), Detach (2),
//  AttachFromTable (3), GetTrackedAgent (4). Declared in ascending address
//  order below to match; nothing here calls anything else in this file by a
//  direct call (GetAgent reaches GetTrackedAgent only through `this`'s own
//  vtable, a real indirect dispatch), so nothing is at risk of being
//  inlined across the group.
//
//  Moved here from Pathfinding/positiontrackcore.cpp (`PositionTrack::*`)
//  once the real shipped hierarchy (TRefCnt -> CPresenceRef -> CAgentField
//  -> Position -> SmartPosition/ProjectilePosition) was confirmed from
//  IDA's own RTTI base-class list - these four addresses are CAgentField's
//  own virtuals, inherited unmodified by Position and both its subclasses,
//  not Position's own. Same addresses, same bodies, same verdicts as
//  before; see Agent/agentfield.h.
//============================================================================
#include "agentfield.h"

//  0x6F4A6970 - Agent/agentslot5cgate.cpp. Declared, not defined, here -
//  same as agent_slot5c.cpp's own local declaration - so this reaches it
//  with the real out-of-line call the shipped code makes.
void __fastcall AgentSlot5CGate(CHandleObject* obj);

//----------------------------------------------------------------------------
//  0x6F478820 - slot 5. Calls slot 4 *through this's own vtable*, not by a
//  direct call to GetTrackedAgent - the shipped code reads `[ecx]` and
//  dispatches `[vtbl+0x10]` itself, so this has to be a genuine virtual
//  call for the instruction stream to match, even though nothing in this
//  family currently overrides slot 4.
//----------------------------------------------------------------------------
CAgent* CAgentField::GetAgent()
{
    SAgileAgent* owner = this->GetTrackedAgent();
    if (owner == 0)
        return 0;
    return owner->m_agent;
}

//----------------------------------------------------------------------------
//  0x6F478920 - slot 2. Resolve the handle pair; if it resolves, tell the
//  resolved object's own rendering-side gate about the detach; then
//  invalidate both fields to -1. The shipped store order is handle
//  (+0x08) then typeTag (+0x0C) - the *opposite* of the naive "inner
//  assignment stores first" reading of `m_typeTag = m_handle = -1;`, which
//  is exactly how this had to be spelled to get that order: the two stores
//  are independent (neither is observable to the other), so this compiler
//  is free to schedule them in either order regardless of which nested
//  assignment reads as "first", and measurement is what settled it, not
//  the source's own left-to-right reading.
//----------------------------------------------------------------------------
void CAgentField::Detach()
{
    CHandleObject* obj = LookupHandle(m_handle, m_typeTag);
    if (obj != 0)
        AgentSlot5CGate(obj);

    m_typeTag = m_handle = (unsigned int)-1;
}

//----------------------------------------------------------------------------
//  0x6F4788C0 - slot 3. Not a LookupHandle-style resolve: `this` is only
//  touched at the very end, once a live entry has actually been found.
//  Fetch bucket `index` out of `table`, and only when both the bounds check
//  and the bucket pointer itself are non-null, copy the entry's own
//  {handle,typeTag} pair onto this object's. The refcount bump happens
//  whenever the bucket slot exists at all, whether or not its own value was
//  null - two separate tests on what reads as the same value because the
//  shipped code re-reads `table->m_buckets[index]` through the address it
//  just took, rather than caching the first read across the branch.
//----------------------------------------------------------------------------
void CAgentField::AttachFromTable(int index, SIndexedHandleTable* table)
{
    if ((unsigned int)index < table->m_count)
    {
        if (table->m_buckets[index] != 0)
        {
            SIndexedTableEntry* entry = table->m_buckets[index];
            if (entry != 0)
            {
                m_handle = entry->m_handle;
                m_typeTag = entry->m_typeTag;
            }
            ++m_refcount;
        }
    }
}

//----------------------------------------------------------------------------
//  0x6F478900 - slot 4. Resolve the handle; if it resolved, return the
//  CPathTrace's own agile-agent registration (+0x30); otherwise null.
//----------------------------------------------------------------------------
SAgileAgent* CAgentField::GetTrackedAgent()
{
    CHandleObject* obj = LookupHandle(m_handle, m_typeTag);
    if (obj == 0)
        return 0;
    return ((CPathTrace*)obj)->m_owner;
}
