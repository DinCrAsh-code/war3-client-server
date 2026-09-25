//============================================================================
//  0x6F4244E0 / 0x6F422D10 / 0x6F421A80 - the player record's own "ability
//  agent tracker" sub-object, reached from PlayerRecordChecksumHash
//  (playerrecordchecksumhash.cpp) through the pointer stored at
//  record+0x34.  Three functions, one shipped module (0x6F42xxxx).
//
//  What this object actually is, and the CUnit* question
//  --------------------------------------------------------------------
//  This object carries at least two intrusive lists (Storm's ordinary
//  {..,next,data} triple, the same shape TSExplicitList and every other
//  list in this repo use, walked from a head/count field that is 0 when
//  empty and otherwise IS the head node's own address):
//
//    * +0x0C/+0x10 - a list of generic entries, each node's own +0x8
//      "datum" pointer treated as a CAgent* (its +0xC/+0x10 are hashed as
//      CAgent::m_typeTag ^ CAgent::m_handle - agent.h's own field layout,
//      confirmed exact by offset).  No type filter at all: whatever is in
//      this list is unconditionally folded in.
//    * +0x1B4/+0x1B8, plus a "current" pointer at +0x1C0 - a second list,
//      each of whose *own* entries (found via ability-list-entry+8) is
//      itself the head of a *third*, per-entry candidate list that
//      SAbilityAgentCollector::CollectAgents below drains through
//      SCheckedUnitSlot::Assign - and SCheckedUnitSlot "*is* the binary's
//      CAgentPtr<CUnit>" per agenttypedslots.h's own note (its Assign
//      calls GetUnitTypeFourCC, 0x6F26C1C0, which returns the literal
//      '+w3u' tag agenttypedslots.h's own table gives CUnit).  So *this*
//      third, per-ability-entry list is definitively filtered down to
//      real CUnit instances before being hashed - the one place in this
//      whole 36-function closure where "CUnit specifically" is not an
//      inference, it is what the type-check tag says.
//
//  Read together, this reads like a per-player "which units can each of
//  my active abilities currently target/carry" cache - CollectAgents
//  drains a *fresh* per-ability candidate list into a scratch array every
//  time this function runs and hashes the CUnit-filtered result, then
//  throws the scratch array away - not a persistent "this player's units"
//  registry.  See docs/notes/checksum-provider-registry.md's own section
//  on this batch for the caveat: the *other* list (+0x0C/+0x10, and the
//  two direct pointers at +0x1A4/+0x1A8 below) is never filtered, so it is
//  evidenced to hold *some* CAgent-derived object - which, since CUnit
//  ultimately derives CAgent through CAgentWar3/CWar3Image/CWidget/
//  CSelectable (unit.h / war3image.h / widget.h / selectable.h / agent.h),
//  is consistent with holding CUnit* too, but nothing in this dump proves
//  it is *only* CUnit* the way the CollectAgents path does.
//
//  SEH-framed in the shipped body (push -1 / push offset SEH_6F4244E0 /
//  mov eax, fs:0 prologue, __CxxFrameHandler3-shaped, two
//  TSGrowableArray<CAgentPtr>-shaped locals crossing the frame) - the same
//  unreproducible __except_handler4-shaped ceiling
//  docs/msvc-vc8-idioms.md already documents (the CAgentPtr/
//  TSGrowableArray example; AcquireGameSingleton and CGameWar3::CGameWar3
//  are this repo's own precedent for scoring this shape as DIFFERS rather
//  than chasing the frame itself).  Only the happy-path fold below is
//  reconstructible.
//
//  2026-09-13 desync investigation - CollectAgents is NOT a spatial query.
//  --------------------------------------------------------------------
//  This closure was flagged as a strong candidate for the live
//  SetUnitPosition per-tick checksum desync (CNetData::FireTickNotification,
//  Net/netcommand_dispatch.cpp) on the theory that CollectAgents might be a
//  live "which units are within range of this ability" spatial query, whose
//  candidate set would shift on whichever client actually moved a unit.
//  Read against the real disassembly, that theory does not hold:
//
//    * CollectAgents itself (0x6F421A80, this file, below) does nothing but
//      walk an *already-populated* intrusive list at source+0xC and
//      type-check each entry via SCheckedUnitSlot::Assign -> GetUnitTypeFourCC
//      (0x6F26C1C0).  No world-position field, no CPathRef/CUnit coordinate,
//      no spatial-grid pointer and no float comparison appears anywhere in
//      its body.
//    * The two known writers of the +0x1B4/+0x1C0 list this checksum reads
//      (both traced instruction-for-instruction) are equally non-spatial:
//      SPlayerSubgroupTarget::NotifyAsyncRefreshSubgroups (0x6F423D40,
//      Item/unitsubgrouprefresh.cpp) and its own per-candidate predicate
//      SAbilityAgentPredicate::CheckAgent (0x6F423500) filter and reorder
//      candidates purely by pointer identity, refcount bookkeeping and
//      AgileTypeIsDerivedFrom/kAgentKindTagAgl type-tag checks - never a
//      coordinate. NotifyAsyncRefreshSubgroups's own name and the
//      ".\\CSelectionWar3.cpp"/"Async sel subgroup by unit ..." strings its
//      callees carry identify this whole apparatus as the player's UI
//      *selection-subgroup* grouping (the tabbed group-by-unit-type view
//      when multiple unit types are selected), not a live ability-targeting
//      query. Its one caller found in this dump set,
//      CAbilityInventory::DetachAndNotify (0x6F0F0F80,
//      Item/... - fires when a unit's inventory is left with no valid
//      handles), is itself an inventory-slot-count check against
//      dword_6FAAE470, unrelated to position.
//    * CUnit's own SAbilityAgentCollector sub-objects
//      (CUnit::CollectAbilityAgentsAt1E8/At1C4, +0x1E8/+0x1C4,
//      Unit/unit_slot115closureleaves.cpp) are wrappers over this same,
//      already-audited CollectAgents - so wherever a unit's per-ability
//      candidate membership is actually *populated* (order issue, ability
//      cast, etc. - not traced here, out of this closure's scope), the read
//      side this checksum folds is confirmed non-spatial regardless.
//
//  Verdict: this leg of PlayerRecordChecksumHash is a pure logical/identity
//  filter (list membership + agile-type tag), not a proximity/range query,
//  and is not evidenced to read any unit's world position. Ruled out as the
//  SetUnitPosition desync mechanism from this angle; see
//  docs/notes/setunitposition-desync-abilityagenttracker.md for the full
//  write-up and citations.
//============================================================================
#include "agenttypedslots.h"
#include "storm.h"

//  Same class unitsubgrouprefresh.cpp's own SPlayerSubgroupTarget (the
//  object at the player-slot table's own +0x34 - literally
//  PlayerRecordChecksumHash's record+0x34 pointer, one field earlier: that
//  file names +0x1AC onward, everything this file reads (+0xC/+0x10,
//  +0x1B4/+0x1B8/+0x1C0, the ten +0x20 slots, +0x1A4/+0x1A8) sits inside
//  its own "m_reserved000[0x1AC]" and "m_reserved1B0" gaps).  Declared
//  here only as precisely as this file's own two call sites need -
//  unitsubgrouprefresh.h keeps the named view.
struct SAbilityAgentCollector
{
    void CollectAgents(TSGrowableArray<CAgentPtr>* out);
};

//  0x6F422D10 - find the 0-based index of `self`'s own "current" pointer
//  (+0x1C0) within the intrusive list at +0x1B4/+0x1B8, or -1 if the
//  current pointer is null, the list is empty, or nothing matches.  No SEH
//  frame in the shipped body - a plain leaf, reconstructed in full.
int __fastcall PlayerAbilityAgentTrackerIndexOfCurrent(void* selfPtr)
{
    char* self = (char*)selfPtr;

    void* current = *(void**)(self + 0x1C0);
    if (current == 0)
        return -1;

    int raw = *(int*)(self + 0x1B8);
    int entry = raw > 0 ? raw : 0;
    if (entry <= 0)
        return -1;

    //  The shipped loop keeps this sentinel (&self+0x1B4, the list's own
    //  head) ready for an `entry == 0` mid-loop case that this closure's
    //  own clamp above already makes unreachable on entry - the same dead
    //  branch every other tail-linked-list walk in this dump's family
    //  carries (see Player/playerscore.h's own Tail()/Prev() note).
    //  Written out because the shipped stream has it, not because
    //  anything here can actually take it.
    int sentinel = (int)(self + 0x1B4);

    int index = 0;
    for (;;)
    {
        if ((void*)(entry + 8) == current)
            return index;

        index++;

        if (entry == 0)
            entry = sentinel;

        entry = *(int*)(entry + 4);
        if (entry <= 0)
            return -1;
    }
}

//  0x6F421A80 - drain `source`'s own candidate list (head/count at
//  source+0xC, an allocation-size hint at source+0x10) into `dest`,
//  keeping only candidates whose runtime agile type derives from CUnit
//  (SCheckedUnitSlot::Assign, checkedunitslot.cpp).  `retn 4`.
//
//  The shipped body's own first three instructions are a call into the
//  exact address this repo's TSGrowableArray<CAgentPtr>::~TSGrowableArray
//  (agentptrarray.cpp) already occupies, immediately followed by an
//  explicit re-zero of `dest`'s own alloc/count/data fields - a "make sure
//  the caller-supplied scratch array starts genuinely empty" guard that
//  reads, in source, as a defensive
//  `dest->~TSGrowableArray<SCheckedUnitSlot>(); dest->m_alloc = dest->m_count = dest->m_data = 0;`
//  SCheckedUnitSlot is bit-for-bit the same shape as CAgentPtr (both a
//  single ref-counted CAgent*), so the shipped, whole-program-optimised
//  build folded the two instantiations' destructors onto one address;
//  this rebuild has no such fold (no /GL here), so the call this emits
//  targets a different, but equally real, address - the same
//  cross-instantiation address drift already established throughout
//  Containers/tshash.inl's own Ptr()/NewNode()/Initialize() siblings.
void SAbilityAgentCollector::CollectAgents(TSGrowableArray<CAgentPtr>* out)
{
    //  Internally, this is a TSGrowableArray<SCheckedUnitSlot> - see the
    //  declaration's own note.  Reinterpreted rather than redeclared so
    //  every caller of this method (this file's own two, and
    //  unitsubgrouprefresh.cpp's NotifyAsyncRefreshSubgroups) keeps one
    //  mangled symbol.
    TSGrowableArray<SCheckedUnitSlot>* dest = (TSGrowableArray<SCheckedUnitSlot>*)out;

    ((TSGrowableArray<CAgentPtr>*)dest)->~TSGrowableArray();
    dest->m_alloc = 0;
    dest->m_count = 0;
    dest->m_data  = 0;

    char* source = (char*)this;
    unsigned int allocHint = *(unsigned int*)(source + 0x10);
    if (allocHint != 0)
        dest->SetAlloc(allocHint);

    int node = *(int*)(source + 0xC);
    if (node > 0)
    {
        do
        {
            void* candidate = *(void**)(node + 8);
            if (candidate)
            {
                SCheckedUnitSlot* slot = dest->New();
                slot->Assign((CAgent*)candidate);
            }

            node = *(int*)(node + 4);
        } while (node > 0);
    }
}

//  0x6F4244E0.
unsigned int __fastcall PlayerAbilityAgentTrackerChecksum(void* selfPtr)
{
    char* self = (char*)selfPtr;
    unsigned int hash = 0;

    //  First list: +0x0C/+0x10, no type filter - each live node's own
    //  datum treated as a CAgent* and hashed by handle^typeTag.
    int node = *(int*)(self + 0xC);
    if (node > 0)
    {
        do
        {
            void* datum = *(void**)(node + 8);
            if (datum)
            {
                unsigned int c = *(unsigned int*)((char*)datum + 0x10) ^ *(unsigned int*)((char*)datum + 0xC);
                c = (c << 3) | (c >> 29);
                hash ^= c;
                hash = (hash << 3) | (hash >> 29);
            }

            node = *(int*)(node + 4);
        } while (node > 0);
    }

    //  This object's own "index of current" contribution.
    {
        unsigned int idx = (unsigned int)PlayerAbilityAgentTrackerIndexOfCurrent(self);
        hash ^= idx;
        hash = (hash << 3) | (hash >> 29);
    }

    //  Second list (+0x1B4/+0x1B8): for each ability-entry, collect its
    //  own CUnit-filtered candidates fresh (SAbilityAgentCollector::
    //  CollectAgents) and fold every collected CUnit's handle^typeTag in,
    //  then release the scratch array before moving to the next
    //  ability-entry.
    int abilityEntry = *(int*)(self + 0x1B8);
    if (abilityEntry > 0)
    {
        do
        {
            TSGrowableArray<CAgentPtr> collected;
            collected.m_alloc = 0;
            collected.m_count = 0;
            collected.m_data = 0;
            collected.m_chunk = 0;
            ((SAbilityAgentCollector*)(abilityEntry + 8))->CollectAgents(&collected);

            for (unsigned int i = 0; i < collected.m_count; i++)
            {
                CAgent* unit = collected.m_data[i].m_ptr;
                if (unit)
                {
                    unsigned int c = *(unsigned int*)((char*)unit + 0xC) ^ *(unsigned int*)((char*)unit + 0x10);
                    c = (c << 3) | (c >> 29);
                    hash ^= c;
                    hash = (hash << 3) | (hash >> 29);
                }
            }

            collected.~TSGrowableArray();

            abilityEntry = *(int*)(abilityEntry + 4);
        } while (abilityEntry > 0);
    }

    //  Ten fixed 0x14-byte slots at +0x20: each is its own tail-linked
    //  list, walked and folded the same way as the +0x0C/+0x10 list above.
    char* slot = self + 0x20;
    for (int i = 0; i < 10; i++)
    {
        int entry = *(int*)slot;
        while (entry > 0)
        {
            void* datum = *(void**)(entry + 8);
            if (datum)
            {
                unsigned int c = *(unsigned int*)((char*)datum + 0x10) ^ *(unsigned int*)((char*)datum + 0xC);
                c = (c << 3) | (c >> 29);
                hash ^= c;
                hash = (hash << 3) | (hash >> 29);
            }

            entry = *(int*)(entry + 4);
        }

        slot += 0x14;
    }

    //  Two direct agent pointers at +0x1A4/+0x1A8.
    void* agentA = *(void**)(self + 0x1A4);
    if (agentA)
    {
        unsigned int c = *(unsigned int*)((char*)agentA + 0x10) ^ *(unsigned int*)((char*)agentA + 0xC);
        c = (c << 3) | (c >> 29);
        hash ^= c;
        hash = (hash << 3) | (hash >> 29);
    }

    void* agentB = *(void**)(self + 0x1A8);
    if (agentB)
    {
        unsigned int c = *(unsigned int*)((char*)agentB + 0x10) ^ *(unsigned int*)((char*)agentB + 0xC);
        c = (c << 3) | (c >> 29);
        hash ^= c;
        hash = (hash << 3) | (hash >> 29);
    }

    return hash;
}
