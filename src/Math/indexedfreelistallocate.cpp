//============================================================================
//  0x6F4BC2B0/0x6F4BC240 - the two "hand out a slot" halves of the indexed
//  free-list family PushIndexedFreeListEntry/STwoIndexedFreeLists::Push
//  (indexedfreelistentry.cpp/indexedfreelistpush.cpp) already give back.
//
//  Both write the identical 8-byte record - {0xFFFFFFFE, payload}, the
//  exact "populated" tag TempestProviderListHash checks for
//  (docs/notes/checksum-provider-registry.md) - into `owner->m_records
//  [index]` and bump `*liveCount`; they differ only in how `index` is
//  chosen: GrowAndStoreFreeListEntry is handed one explicitly and only
//  grows storage if that index is not inside it yet, while
//  PopFreeSlotOrGrowAndStore either grows by exactly one (index = the
//  array's own pre-growth capacity) or reuses whatever the free list's own
//  head currently points at.
//
//  See indexedfreelist.h's own correction note: on at least one of the two
//  known receiver objects for this pair (`g_unk6FAB778C`, the tempest
//  presence host), `owner`'s enclosing object is now RTTI-confirmed to be
//  `NTempest::CDynTable<CPrRgEntry>` rather than an untyped allocator -
//  this file's own instructions are unaffected either way.
//
//  Left in one translation unit together: neither calls the other (both
//  call only EnsureCapacityForGrowth, an out-of-scope naked thunk into
//  0x6F486780 - see indexedfreelist.h), so there is no real cross-call
//  here for this build's /Ob2 to inline away.  0x6F4BC310
//  (STwoIndexedFreeLists::AllocateAt), which calls both of these for real,
//  is its own translation unit for exactly that reason -
//  indexedfreelistallocatedispatch.cpp.
//============================================================================
#include "indexedfreelist.h"

__declspec(naked) void __thiscall SIndexedFreeListOwner::EnsureCapacityForGrowth(
    const SIndexedFreeListRecord* /*fillValue*/, int /*growBy*/)
{
    __asm { mov eax, 06F486780h }
    __asm { jmp eax }
}

int __stdcall GrowAndStoreFreeListEntry(void* payload, SIndexedFreeListOwner* owner,
                                        int* /*headUnused*/, int* liveCount, int index)
{
    if ((unsigned int)index >= owner->m_capacity)
    {
        SIndexedFreeListRecord fillValue = { -1, 0 };
        owner->EnsureCapacityForGrowth(&fillValue, index - owner->m_capacity + 1);
    }

    SIndexedFreeListRecord* slot = &owner->m_records[index];
    slot->m_field0 = (int)0xFFFFFFFE;
    slot->m_field4 = (int)payload;
    *liveCount += 1;

    return index;
}

int __stdcall PopFreeSlotOrGrowAndStore(void* payload, SIndexedFreeListOwner* owner,
                                        int* headIndex, int* liveCount)
{
    int index = *headIndex;

    if (index == -1)
    {
        index = owner->m_capacity;
        SIndexedFreeListRecord fillValue = { (int)0xFFFFFFFE, (int)payload };
        owner->EnsureCapacityForGrowth(&fillValue, 1);
    }
    else
    {
        SIndexedFreeListRecord* slot = &owner->m_records[index];
        *headIndex = slot->m_field0;
        slot->m_field0 = (int)0xFFFFFFFE;
        slot->m_field4 = (int)payload;
    }

    *liveCount += 1;
    return index;
}
