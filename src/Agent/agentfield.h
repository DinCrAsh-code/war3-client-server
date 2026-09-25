//============================================================================
//  CAgentField - `??_7CAgentField@@6B@`, RTTI-verified
//  (agent_worktrees/classes/0x6F876D48.json). `CAgentField : public
//  CPresenceRef` (presenceref.h) - see that file's own header comment for
//  the full chain and why this correction exists.
//
//  Slots 2-5 (Detach/AttachFromTable/GetTrackedAgent/GetAgent) are real,
//  byte-identical reconstructions - moved here from Pathfinding/
//  positiontrack.h's own `PositionTrack`, which originally declared them
//  as its own before this class's existence was known. Same addresses,
//  same bodies, same EXACT/DIFFERS verdicts as before - this is a pure
//  attribution fix, not new reconstruction work. `{m_handle, m_typeTag}`
//  stay on `CPresenceRef` (presenceref.h), one level up - this class adds
//  no fields of its own, only these four vtable slots plus its own slot-1
//  override; `PositionTrack` (and `SmartPositionTrack`/
//  `ProjectilePositionTrack` below it) see the fields exactly as before
//  through ordinary public inheritance either way.
//
//  Slot 1 (this class's own DeleteSelf override, 0x6F022610) is real now
//  too (agentfield-presenceref follow-up pass). See
//  agentfield_deleteself.cpp.
//============================================================================
#ifndef AGENTFIELD_H
#define AGENTFIELD_H

#include "presenceref.h"
#include "game.h"        // CHandleObject, LookupHandle, CPathTrace
#include "agentquery.h"  // SAgileAgent

class CAgent;   // Agent/agent.h

//----------------------------------------------------------------------------
//  The hash-bucket-style table AttachFromTable's second argument names.
//  Only the two members either call tree reads are named; nothing here
//  establishes a real type for the rest of either struct.
//----------------------------------------------------------------------------
struct SIndexedTableEntry
{
    char         m_reserved00[0x14];
    unsigned int m_handle;    // +0x14 - installed onto the caller's own m_handle
    unsigned int m_typeTag;   // +0x18 - installed onto the caller's own m_typeTag
};

struct SIndexedHandleTable
{
    char                 m_reserved00[0x64];
    SIndexedTableEntry** m_buckets;   // +0x64
    char                 m_reserved68[0x74 - 0x68];
    unsigned int         m_count;     // +0x74
};

class CAgentField : public CPresenceRef
{
public:
    //  slot 1 / +0x04 (0x6F022610) - real. See agentfield_deleteself.cpp.
    virtual CAgentField* DeleteSelf(int flags);

    //  slot 2 / +0x08 (0x6F478920) - resolve the handle pair; if it
    //  resolves, tell the resolved object's own rendering-side gate
    //  (AgentSlot5CGate, Agent/agentslot5cgate.cpp) about the detach; then
    //  invalidate both fields to -1. See agentfield_core.cpp.
    virtual void Detach();

    //  slot 3 / +0x0C (0x6F4788C0) - install a fresh handle pair by
    //  fetching bucket `index` out of `table` and bump the refcount. Not a
    //  `(this, args...)` LookupHandle-style resolve - `this` is only
    //  touched at the very end. Overridden by ProjectilePositionTrack
    //  (Pathfinding/projectilepositiontrack.h). See agentfield_core.cpp.
    virtual void AttachFromTable(int index, SIndexedHandleTable* table);

    //  slot 4 / +0x10 (0x6F478900) - resolve the handle, then return the
    //  resolved CPathTrace's own agile-agent registration
    //  (CPathTrace::m_owner, game.h), or null if the handle didn't
    //  resolve. Declared virtual because slot 5 below reaches it through
    //  `this`'s own vtable, not by a direct call. See agentfield_core.cpp.
    virtual SAgileAgent* GetTrackedAgent();

    //  slot 5 / +0x14 (0x6F478820) - GetTrackedAgent() through `this`'s own
    //  vtable, then that registration's own live CAgent*
    //  (SAgileAgent::m_agent, +0x54, agentquery.h), or null either way it
    //  came up empty. See agentfield_core.cpp.
    virtual CAgent* GetAgent();
};

#endif
