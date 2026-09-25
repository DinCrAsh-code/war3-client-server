//============================================================================
//  jscript-bin3's own worklist - a cluster of `TAllocatorHashTable`/
//  `TSExplicitList`-shaped internals in the JASS VM module's own address
//  range (0x6F448000-0x6F467000, the same neighbourhood
//  docs/targets/JassVM_ModuleSweep.md already swept).  Every one of these
//  reads like a genuine hash-table/array-management member (bucket
//  rehash/grow, node lookup-or-insert, node unlink), but each either calls
//  a still-unreconstructed sibling in the same family (sub_6F453F50,
//  sub_6F44C710, sub_6F44C780, sub_6F45ABA0, sub_6F459470, sub_6F45B330,
//  sub_6F45B960, sub_6F45B900, sub_6F45C480, sub_6F45A3D0, sub_6F459710,
//  sub_6F44CFC0) or dispatches through a node's own vtable slot 0 (the
//  deleting-destructor shape jasshashtablectors.cpp already documents this
//  repo cannot model without stamping a vtable this build does not own) -
//  so each stays a self-redirect thunk rather than a guess at the missing
//  half.
//============================================================================

#define SELF_REDIRECT(addr)          \
    {                                 \
        __asm { mov eax, addr }      \
        __asm { jmp eax }             \
    }

//  0x6F44B860 - `retn 4`, no register args (the object arrives on the
//  stack).  Unlinks two embedded TSLink<T>-shaped members (at +0x04 and
//  +0x0C) guarded by one shared "is this parked in a list at all" test on
//  the first link's own m_prevlink - the same inline shape storm.h's
//  TSLink<T>::Unlink() already models, just gating both unlinks at once
//  rather than the usual one-at-a-time `if (m_next)` per link, which is not
//  provably safe to reproduce without knowing why the two links are always
//  linked/unlinked together.
__declspec(naked) void __stdcall UnlinkTwoEmbeddedLinks(void* obj)
    SELF_REDIRECT(06F44B860h)

//  0x6F44CEC0 - `retn 4` (__thiscall).  TSHashTable-shaped Destruct(): walks
//  every bucket's node list, unlinking each (sub_6F44C340-family relink) and
//  - gated on the stack flag - releasing it through the node's own vtable
//  slot 0 (`call [eax]`).  Same "unmodelled node vtable" reason
//  Containers/hashtablectors.cpp already gives for every other Destruct() in
//  this codebase.
__declspec(naked) void TAllocHashTableDestructVariant1(void* self, int)
    SELF_REDIRECT(06F44CEC0h)

//  0x6F44D4D0 - `retn 4` (__thiscall).  Instruction-for-instruction the same
//  shape as TAllocHashTableDestructVariant1 above, a distinct instantiation
//  (calls sub_6F44C780 rather than sub_6F44C340 for its own bucket count).
__declspec(naked) void TAllocHashTableDestructVariant2(void* self, int)
    SELF_REDIRECT(06F44D4D0h)

//  0x6F44ECD0 - `retn 0` (__thiscall).  The same node-vtable-dispatch
//  Destruct() shape over a *flat* array of buckets (no `+0x1C` indirection),
//  finishing with a Storm_403 free of the bucket array itself tagged
//  "AVTSExplicitLi..." (a TSGrowableArray<TSExplicitList<T>>'s own RTTI
//  name) - the array-of-buckets half of the same family the two functions
//  above cover for the hash table proper.
__declspec(naked) void TSExplicitListBucketArrayDestruct(void* self)
    SELF_REDIRECT(06F44ECD0h)

//  0x6F454630 - `retn 0` (__thiscall).  Frees two owned buffers tagged
//  "delete" through Storm_403 after releasing one of them via
//  sub_6F453F50 first (an unreconstructed sibling), then zeroes both
//  fields - a small object's own cleanup, not a hash-table member.
__declspec(naked) void ReleaseTwoDeleteTaggedBuffers(void* self)
    SELF_REDIRECT(06F454630h)

//  0x6F454850 - `retn 0Ch` (__thiscall, 3 stack args).  Hashes a
//  caller-supplied key (Storm_590) and creates a table node for it
//  (sub_6F4541B0 - already a real, defined TSHashTable<JassInstanceSlotC,
//  HASHKEY_NONE>::NewNode instantiation per jassinstancetablec.cpp), then
//  stamps the new node's own m_hashval/m_key fields - the same shape
//  Containers/tshash.inl's TSHashTable<T,K>::New already gives a different
//  instantiation at a different address, just with the hash computed here
//  rather than passed in.  Left a redirect because this repo has no
//  existing "hash-then-New" member to reuse and confirming JassInstanceSlotC
//  really carries an m_key at the byte offset this dump writes to (given
//  HASHKEY_NONE's own near-zero footprint) needs more evidence than this
//  closure alone provides.
__declspec(naked) void* JassInstanceSlotDeclare(void* table, const void*,
                                                void*, void*)
    SELF_REDIRECT(06F454850h)

//  0x6F45A460 - `retn 10h` (__thiscall, 4 stack args).  A guarded insert
//  into a table at `this+0x20` (sub_6F459710) preceded by a grow check
//  (sub_6F45A3D0) - the same CheckGrow-then-insert shape as
//  TAllocatorHashTable<T,K,N>::NewNode, over a table this closure cannot
//  pin an instantiation for.
__declspec(naked) void GuardedTableInsert(void* self, unsigned int,
                                          unsigned int, unsigned int)
    SELF_REDIRECT(06F45A460h)

//  0x6F45B600 - `retn 4` (__thiscall, 1 stack arg).  A bucket-array
//  Rehash()-shaped resize: grows to at least the stack argument's own
//  count, computing a divisor via sub_6F459470 when none is cached, then
//  re-links every existing bucket's nodes (the same poisoned-link-offset
//  TSExplicitList construction Containers/hashtablectors.cpp documents) via
//  sub_6F44C710. Left a redirect: the divisor/modulo step does not match
//  Containers/allochash.inl's own TAllocatorHashTable<T,K,N>::Rehash
//  (power-of-two doubling, no division), so this is a genuinely different
//  table shape this closure does not otherwise reach.
__declspec(naked) void TableGrowToCapacity(void* self, unsigned int)
    SELF_REDIRECT(06F45B600h)

//  0x6F45B720 - `retn 4` (__thiscall, 1 stack arg).  Walks a linked chain
//  off `this+0x9C`/`this+0x94`, conditionally calling sub_6F45B2E0 on each
//  node (the JASS native-vs-user dispatch family jassvm-core-A's own
//  session already names DispatchWithOwnDecl at a different address - this
//  looks like a sibling walk over the same shape but is not confirmed to be
//  the same instantiation), then tears the chain down via sub_6F44CFC0.
__declspec(naked) void WalkAndDispatchChain(void* self, void*)
    SELF_REDIRECT(06F45B720h)

//  0x6F45B7E0 - `retn 0` (__thiscall).  Re-initialises a 4-entry array at
//  `this+0x1C` (sub_6F45B330), resetting each element's own field to a
//  poisoned-link-offset TSExplicitList<T> the same way Rehash's own parked
//  list is born (sub_6F44C780).
__declspec(naked) void ResetFourEntryArray(void* self)
    SELF_REDIRECT(06F45B7E0h)

//  0x6F45BED0 - `retn 4` (__thiscall, 1 stack arg).  A bucket-move/rehash
//  body: builds a local TSExplicitList (sub_6F44C710), parks every node of
//  `this`'s own +0x1C bucket array onto it, resizes via TableGrowToCapacity
//  above, then re-inserts every parked node by its own stored hash - the
//  same Rehash() shape TableGrowToCapacity itself is one half of.
__declspec(naked) void TableRehashAndRelink(void* self, unsigned int)
    SELF_REDIRECT(06F45BED0h)

//  0x6F45C2A0 - `retn 4` (__thiscall, 1 stack arg).  A CheckGrow()-shaped
//  probe-count walk over `this+0x1C`'s own bucket at the stack argument's
//  index, growing `this+0x24`'s own generation counter and, past a 13-probe
//  cap, calling sub_6F45B960 to actually resize - the same decaying
//  probe-count idiom Containers/allochash.inl's own CheckGrow documents,
//  over a table shape this closure does not otherwise pin down.
__declspec(naked) int TableCheckGrowVariant(void* self, unsigned int)
    SELF_REDIRECT(06F45C2A0h)

//  0x6F45C770 - `retn 0Ch` (__thiscall, 3 stack args).  A find-or-insert:
//  looks a key up (sub_6F45C480), lazily rehashes on the miss path
//  (sub_6F45B900), then either returns a match or splices a fresh node into
//  its bucket and the table-wide list - the same Ptr()/insert shape
//  Containers/allochash.inl's own TAllocatorHashTable<T,K,N>::Ptr documents,
//  over a distinct instantiation this closure cannot confirm.
__declspec(naked) void* FindOrInsertTableEntry(void* self, unsigned int,
                                               void*, void*)
    SELF_REDIRECT(06F45C770h)

//  0x6F45E760 - `retn 4` (__thiscall, 1 stack arg).  A slot-validity guard:
//  computes a slot address from the stack index (`this + (index+2)*0x28`),
//  and - only when a flag at `this+0x48` is clear or the slot's own +0x18
//  is nonzero - forwards to sub_6F45E6D0.  Otherwise returns false without
//  calling it.
__declspec(naked) int TableSlotIsUsable(void* self, unsigned int)
    SELF_REDIRECT(06F45E760h)

#undef SELF_REDIRECT
