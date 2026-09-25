//============================================================================
//  0x6F4BC310 - STwoIndexedFreeLists::AllocateAt.  See indexedfreelist.h
//  for the full pool-selection rule and the two helpers this calls.
//
//  See indexedfreelist.h's own correction note: a live breakpoint here with
//  `this` == `g_unk6FAB778C` (Pathfinding/gridregistrationteardown.cpp)
//  RTTI-resolved that receiver to `NTempest::CDynTable<CPrRgEntry>`. This
//  function's own reconstructed instructions do not change - only which
//  concrete class one of its two known callers' receivers really is.
//
//  Own translation unit: this is the one function in the family with a
//  real cross-call to *two* other siblings (GrowAndStoreFreeListEntry and
//  PopFreeSlotOrGrowAndStore, indexedfreelistallocate.cpp) - keeping it
//  alongside either of them would let this build's /Ob2 inline that one
//  away, the same reasoning indexedfreelistpush.cpp's own header comment
//  already gives for PushIndexedFreeListEntry.
//============================================================================
#include "indexedfreelist.h"

unsigned int __thiscall STwoIndexedFreeLists::AllocateAt(void* payload,
                                                         int allocateFromNegativePool,
                                                         int index)
{
    if (index != -1)
    {
        if ((unsigned int)index & 0x80000000u)
        {
            int absIndex = index & 0x7FFFFFFF;
            return 0x80000000u | (unsigned int)GrowAndStoreFreeListEntry(
                payload, &m_negativeOwner, &m_negativeHead, &m_negativeRemaining, absIndex);
        }

        return (unsigned int)GrowAndStoreFreeListEntry(
            payload, (SIndexedFreeListOwner*)this, &m_positiveHead, &m_positiveRemaining, index);
    }

    if (allocateFromNegativePool)
    {
        return 0x80000000u | (unsigned int)PopFreeSlotOrGrowAndStore(
            payload, &m_negativeOwner, &m_negativeHead, &m_negativeRemaining);
    }

    return (unsigned int)PopFreeSlotOrGrowAndStore(
        payload, (SIndexedFreeListOwner*)this, &m_positiveHead, &m_positiveRemaining);
}
