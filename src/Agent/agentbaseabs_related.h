//============================================================================
//  NIpse::CRlAgent's own "related agents" machinery: the `NTempest::CDynTable<
//  NIpse::CPrRelation*>` this class keeps at object offset +0x58
//  (`CRlAgent::m_relatedAgents`, `src/Pathfinding/crlagent.h`, "capacity 8")
//  and four `SRelationLink` list heads that live inside what that header
//  currently calls `m_ownReserved78[0x40]` (offsets 0x78-0xB7 of `CRlAgent`).
//
//  CORRECTION (2026-09-07, vtable-derived): these accessors (Attach*,
//  Detach*, SetEventId/SetField*, SubmitRelatedAgentEvent, and `Save` below)
//  used to be scoped under a `CAgentBaseAbsRelated` struct on the mistaken
//  assumption that "reached through one of CAgentBaseAbs's 19 vtable slots"
//  meant "CAgentBaseAbs's own method". Walking the full CPresence -> ... ->
//  CAgentBaseAbs slot-target table (built from every class's own
//  `agent_worktrees/classes/<ADDR>.json`, now available after the operator's
//  namespace/template vtable-dump fix) shows CRlAgent, not CAgentBaseAbs, is
//  where every one of these slots' targets actually first changes - and
//  every field this file's own accessors touch (`m_relatedAgents` at +0x58,
//  the four `SRelationLink` heads at +0x78/+0x88/+0x98/+0xA8) is CRlAgent's
//  own field per `crlagent.h`. All of them are declared as real members of
//  `NIpse::CRlAgent` there now instead; this header keeps only the
//  non-owning helper types (the table itself, and the two out-of-batch
//  helper structs below) that were never misattributed.
//
//  **NIpse::CPrRelation and Agent/agentrelation.h's `CAgentRelation` are the
//  same class.** `NIpse::CRlAgent::Save` (0x6F4A4EE0, Pathfinding/crlagent.h,
//  defined in Agent/agentbaseabssave.cpp) calls `sub_6F4A6570` with the
//  *outer* `this` still in `ecx`, unchanged since entry - and
//  `agentrelation.h` independently names that exact address
//  `CAgentRelation::Save`. The only way both are right is if `NIpse::CRlAgent`
//  ( : public NIpse::CPrRelation : ...) really does inherit a
//  `CAgentRelation` at offset 0 - i.e. `CAgentRelation` in that header is
//  this session's confirmation of what `NIpse::CPrRelation` actually is, not
//  a second, unrelated "link" class. This is also why `m_relatedAgents`
//  holds `CPrRelation*` elements (crlagent.h) that this file's own
//  accessors treat as `CAgentRelation*` throughout: they are the same
//  pointer.
//
//  **This investigation corrects crlagent.h's own placeholder
//  `CDynTable_CPrRelationPtr` field guess.** That struct (written from the
//  *constructor's* instruction stream alone, and flagged there as
//  "unmodeled... out of scope for this session") assumed a leading vtable
//  pointer and put capacity at +0x14 / count at +0x18. The *readers* this
//  session reconstructed (0x6F4A4FE0, 0x6F4A5060, and every accessor below
//  that reaches the table through the outer object) settle it differently,
//  from real field reads and writes rather than a constructor's opaque
//  stores: relative to `&m_relatedAgents` (i.e. `this+0x58`), the table is
//  `CMemBlock`-shaped with NO extra vtable slot in front of it at all -
//  `+0x00..+0x10` are `CMemBlock`'s own five fields exactly as
//  `Pathfinding/cellbuffer.h` already documents them elsewhere in this
//  binary (`m_reserved00`, `m_block`, `m_blockSize`, `m_data`, `m_size`),
//  `+0x14` is a growth-only field never stored back into by the code this
//  session read (kept unnamed), `+0x18` is the real capacity (grows by the
//  same delta `sub_6F4A4FE0`'s own `SetSize` call requests) and `+0x1C` is
//  the real element count (incremented by the insert count on a successful
//  grow, decremented in `RemoveRange` below). `Save` and every accessor in
//  this file read the *same* two fields through the outer object at
//  `+0x74` (count) and `+0x64` (data pointer) - `0x58+0x1C` and `0x58+0x0C`
//  respectively - which is the cross-check that ties the two addressing
//  styles to one table. `crlagent.h` is not this session's file to edit (it
//  belongs to `CRlAgent`'s own ctor/dtor reconstruction); a later
//  integration pass can fold this correction back into it.
//
//  Two calling conventions appear across the functions this header used to
//  declare: some take `&m_relatedAgents` itself in `ecx` (0x6F4A4FE0,
//  0x6F4A5060 - `CRelatedAgentsTable`, still declared below), the rest take
//  the *outer* `CRlAgent` object and read the table through its
//  `+0x64`/`+0x74` window - those are now declared as real `NIpse::CRlAgent`
//  members in `Pathfinding/crlagent.h` instead (see this file's own header
//  comment's CORRECTION note), since a full `class CRlAgent : ...` already
//  exists there (crlagent.h's own ctor/dtor reconstruction) to hang them
//  on, unlike `CAgentBaseAbs` (only a handful of its 19 vtable slots are
//  reconstructed - docs/targets/vtables/CAgentBaseAbs.txt - so a full class
//  there would still claim a vtable this repo cannot stamp or verify).
//  `CRelatedAgentsTable` below remains its own raw-offset interface in the
//  same spirit as `HashTaggedFieldRecordCollection`
//  (Misc/genericfieldrecordhash.cpp), reached through `&m_relatedAgents`
//  rather than the outer object.
//============================================================================
#ifndef AGENTBASEABS_RELATED_H
#define AGENTBASEABS_RELATED_H

#include "agentrelation.h"

//----------------------------------------------------------------------------
//  `&m_relatedAgents` itself: the `CMemBlock`-shaped growable array of
//  `CPrRelation*`/`CAgentRelation*`.  See this file's header comment for
//  the field-offset evidence.
//----------------------------------------------------------------------------
struct CRelatedAgentsTable
{
    //  0x6F4A4FE0 - insert `count` pointer-sized elements copied from
    //  `*items` (nullable - a null `items` grows/reserves without copying
    //  anything in) at the *end* of the table, growing the backing
    //  `CMemBlock` first if `m_count + count` would exceed `m_capacity`.
    //  A zero `m_growthHint` means this table cannot grow at all (returns 0
    //  immediately rather than even trying `SetSize`) - every other case
    //  grows by `max(m_growthHint, deficit)` slots and asks
    //  `CMemBlock::SetSize` (0x6F4B39C0, Pathfinding/cellbuffer.h) for that
    //  many bytes with `noZero = 1`. Returns 0 only when that grow fails;
    //  the table is left untouched in that case. The actual element
    //  copy/shift into place is `sub_6F4A4AD0` - out of this session's
    //  batch (owned by a parallel session working the same dump),
    //  forward-declared below purely so this translation unit compiles
    //  under `verify.py` (which never links); the real definition lands
    //  from that other session.
    int GrowInsert(const void* items, unsigned int count);

    //  0x6F4A5060 - remove up to `count` elements starting at `index`
    //  (clamped to what is actually left past `index`), sliding the tail
    //  down over the gap with `memmove` and shrinking `m_count`. Returns 0
    //  without touching anything if `index` is already past the last
    //  element.
    int RemoveRange(unsigned int index, unsigned int count);

    unsigned int m_reserved00;   // +0x00 - CMemBlock::m_reserved00
    void*        m_block;        // +0x04 - CMemBlock::m_block
    unsigned int m_blockSize;    // +0x08 - CMemBlock::m_blockSize
    void*        m_data;         // +0x0C - CMemBlock::m_data - the CAgentRelation* array
    unsigned int m_size;         // +0x10 - CMemBlock::m_size
    unsigned int m_growthHint;   // +0x14 - read by GrowInsert, never stored back here
    unsigned int m_capacity;     // +0x18 - element capacity
    unsigned int m_count;        // +0x1C - live element count
};

//  The outer object's own `+0x64`/`+0x74` window (`m_relatedAgents.m_data`/
//  `.m_count`, `this+0x58` plus the `CRelatedAgentsTable` offsets above) and
//  its `+0x78`/`+0x88`/`+0x98`/`+0xA8` `SRelationLink` list heads
//  (agentrelation.h) are what `NIpse::CRlAgent`'s own Attach/Detach/SetField*/
//  SubmitRelatedAgentEvent/Save members (Pathfinding/crlagent.h) address -
//  see this file's own header comment for why they moved there.

//  0x6F4A45E0 / 0x6F4A45B0 - callees of CAgentBaseAbs vtable slot 3
//  (agentbaseabs_slot3.cpp) that are out of this session's batch, owned by
//  a parallel session walking the same dump (0x6F4A6070, the slot's other
//  callee, is now real - `CAgentRelation::ResetEventBinding`,
//  agentbaseabseventbinding.cpp - and agentbaseabs_slot3.cpp calls it
//  directly). Forward-declared here only so this translation unit compiles
//  under `verify.py`, which never links; the real definitions land from
//  that other session, at which point these declarations need to be
//  reconciled against whatever signature it actually gives them
//  (`link_check.py`'s job, not this file's).
struct CAgentBaseAbsSlot3Arg
{
    void* LookUp_6F4A45E0();
};
struct CAgentBaseAbsSlot3Result
{
    void FixUp_6F4A45B0(void* outerAgent);
};

//  0x6F4A4AD0 - the actual element copy/shift `GrowInsert` (0x6F4A4FE0)
//  hands off to once the table is known to have room, out of this
//  session's batch, same reasoning.
struct CRelatedAgentsTableInsertExternal
{
    void InsertAt_6F4A4AD0(unsigned int oldCount, const void* items, unsigned int newCount);
};

#endif
