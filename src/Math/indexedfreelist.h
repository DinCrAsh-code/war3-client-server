//============================================================================
//  Shared declarations for the indexed-free-list push/pop/allocate family
//  (indexedfreelistpush.cpp, indexedfreelistentry.cpp,
//  indexedfreelistallocate.cpp, indexedfreelistallocatedispatch.cpp).
//  Split across this many translation units, not one, because the shipped
//  binary keeps every one of these as a real, separate `call` - see each
//  .cpp's own header comment for which caller/callee pair forced which
//  split (this build has no /GL, so nothing here can ever be inlined
//  across a translation-unit boundary once it is one).
//
//  STwoIndexedFreeLists moved here (was a local struct inside
//  indexedfreelistpush.cpp) purely so indexedfreelistallocatedispatch.cpp
//  can share the one real layout rather than duplicating it - no field or
//  method already declared on it changed.
//
//  CORRECTION (live-debugger RTTI, 2026-09-07): the name is, and stays, a
//  reconstruction-only label for a *byte layout* - "two indexed free lists
//  plus their head/remaining counters" - not a real class in the binary's
//  own RTTI.  At least one concrete object built on this exact layout has
//  now been identified for real: a breakpoint on AllocateAt (0x6F4BC310,
//  below) with `this` == the tempest presence host (`g_unk6FAB778C`,
//  Pathfinding/gridregistrationteardown.cpp) resolved its vtable pointer's
//  RTTI to `NTempest::CDynTable<CPrRgEntry>` (`CMemBlockT<CPrRgEntry> ->
//  CMemBlock -> CEntity` above that - see
//  docs/notes/checksum-provider-registry.md's own "cdyntable-prrgentry-rtti"
//  batch). A templated dynamic table built on a generic two-free-list
//  allocator is exactly the shape this header already modelled, so nothing
//  about the reconstructed *instructions* changes - only the attribution of
//  which real class one specific instance (`g_unk6FAB778C`) turns out to
//  be. This struct is still the right, class-agnostic name for the shared
//  shape itself: `g_pHandleTable` (Pathfinding/handletable.h) is a second,
//  distinct object built on the identical layout and is *not* shown by
//  anything found so far to be a `CDynTable` instantiation - see that
//  header's own note. Do not rename `STwoIndexedFreeLists`, `Push` or
//  `AllocateAt` to a `CDynTable`-specific name: they are still shared,
//  multiply-instantiated code, and only one of their two known receiver
//  objects has a confirmed RTTI identity.
//============================================================================
#ifndef INDEXEDFREELIST_H
#define INDEXEDFREELIST_H

struct SIndexedFreeListRecord
{
    int m_field0;
    int m_field4;
};

//  A single free/allocated-slot array: `m_records` is the backing storage,
//  `m_capacity` how many records it currently has room for.  The same code
//  reads this shape at two different base addresses (see
//  STwoIndexedFreeLists below) - a "positive" pool at `this` itself and a
//  "negative" one at `this+0x20` - which is why it stays its own 0x20-byte
//  struct rather than four loose fields on STwoIndexedFreeLists.
struct SIndexedFreeListOwner
{
    char                    m_reserved00[0xC];
    SIndexedFreeListRecord* m_records;    // +0x0C
    char                    m_reserved10[0xC];
    unsigned int            m_capacity;    // +0x1C

    //  0x6F486780 - TSGrowableArray<SIndexedFreeListRecord>-shaped
    //  EnsureCapacity/grow-by-N: out of scope for the batch that reached
    //  this header (a generic, heavily-reused growable-array internal with
    //  its own further leaves, sub_6F4B39C0/sub_6F4840C0, unrelated to the
    //  checksum-provider-registry question that batch actually chased -
    //  see docs/notes/checksum-provider-registry.md).  Naked redirect;
    //  `retn 8` (two stack dwords: the fill value, then how many records to
    //  grow by), confirmed off its own raw_asm
    //  (agent_worktrees/funcs/0x6F486780.json).
    void __thiscall EnsureCapacityForGrowth(const SIndexedFreeListRecord* fillValue,
                                            int growBy);
};

//  0x6F4BC1D0 - retn 0x14 (five stack dwords, no register argument at all -
//  confirmed by its own dump, which never reads ecx).  `obj->m_records
//  [index]` is an 8-byte record; its first field takes whatever
//  `*headIndex` currently holds (the list's own head), its second is
//  zeroed, `*headIndex` is set to `index` (pushing it to the front) and
//  `*remaining` is decremented.  A classic index-linked free list.  The
//  second argument is never read - confirmed dead by its own dump.
void __stdcall PushIndexedFreeListEntry(int index, int unused,
                                        SIndexedFreeListOwner* obj,
                                        int* headIndex, int* remaining);

//  0x6F4BC2B0 - grow `owner`'s own record array if `index` is not already
//  inside its current capacity, then stamp a freshly "allocated" record
//  (tag 0xFFFFFFFE - the exact "populated" tag TempestProviderListHash
//  checks for, see docs/notes/checksum-provider-registry.md) at
//  `owner->m_records[index]`, with `payload` as its second dword.
//  `*liveCount` is incremented on every call, whether or not growth ran.
//  `headUnused` is passed by every caller (the pool's own free-list head
//  pointer) but never read here - confirmed dead by this function's own
//  dump: writing at an *explicit* index never touches the free list.
//  Returns `index` unchanged; STwoIndexedFreeLists::AllocateAt is what
//  folds a pool-selector bit into the handle it hands back to its own
//  caller.
int __stdcall GrowAndStoreFreeListEntry(void* payload, SIndexedFreeListOwner* owner,
                                        int* headUnused, int* liveCount, int index);

//  0x6F4BC240 - the free-list-aware sibling of the above.  If
//  `*headIndex == -1` (nothing on the free list) it grows the array by one
//  record and stamps the newly appended slot exactly like
//  GrowAndStoreFreeListEntry (the returned index is the array's own
//  pre-growth capacity, i.e. its new highest index); otherwise it pops the
//  head of the free list (`owner->m_records[*headIndex]`, whose own
//  `m_field0` - the next free index - becomes the new `*headIndex`) and
//  reuses that slot.  Either way `*liveCount` is incremented and the
//  reused-or-newly-grown index is returned.
int __stdcall PopFreeSlotOrGrowAndStore(void* payload, SIndexedFreeListOwner* owner,
                                        int* headIndex, int* liveCount);

//----------------------------------------------------------------------------
//  0x6F4BC200/0x6F4BC310 - one object carrying two independent indexed
//  free-list pools, selected by the sign of the index: non-negative
//  indices live in `this` itself (viewed as a SIndexedFreeListOwner*),
//  negative ones (their sign bit masked off before use) in the embedded
//  m_negativeOwner sub-object at +0x20.
//----------------------------------------------------------------------------
struct STwoIndexedFreeLists
{
    char                    m_reserved00[0x20];
    SIndexedFreeListOwner   m_negativeOwner;     // +0x20 (0x20 bytes, to +0x40)
    int                     m_positiveHead;      // +0x40
    int                     m_negativeHead;      // +0x44
    int                     m_positiveRemaining; // +0x48
    int                     m_negativeRemaining; // +0x4C

    //  ecx = this, retn 8 (two stack dwords).
    void __thiscall Push(int index, int unused);

    //  0x6F4BC310 - allocate a fresh record.  `index == -1` means "mint the
    //  next one off a free list/growth" (PopFreeSlotOrGrowAndStore, choosing
    //  the negative pool when `allocateFromNegativePool` is nonzero, the
    //  positive one otherwise); any other `index` means "store at exactly
    //  this absolute slot" (GrowAndStoreFreeListEntry, growing that pool's
    //  storage if needed) - and *that* branch chooses the pool by `index`'s
    //  own sign, independent of `allocateFromNegativePool` (masked off
    //  before use when negative).  The returned handle has its own top bit
    //  set whenever the negative pool was used, in both branches alike -
    //  the same bit sub_6F4AEFF0/sub_6F4AEFC0 (handletable.cpp) read back
    //  from their own copy of the handle to know which pool to release
    //  into later.
    unsigned int __thiscall AllocateAt(void* payload, int allocateFromNegativePool,
                                       int index);
};

//  On `g_unk6FAB778C` (Pathfinding/gridregistrationteardown.cpp) - the
//  receiver whose RTTI a live breakpoint on this AllocateAt actually
//  resolved - `this` is really `NTempest::CDynTable<CPrRgEntry>*`; see the
//  correction note at the top of this header. `g_pHandleTable`
//  (Pathfinding/handletable.h) is a second, still-untyped receiver of the
//  same two methods and is not shown to be that class.

#endif
