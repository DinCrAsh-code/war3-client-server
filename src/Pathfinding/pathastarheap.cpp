//============================================================================
//  0x6F498580 / 0x6F4985F0 - the A* open list proper: a binary min-heap over
//  the SAStarHeapEntry array pathastarheaparray.cpp allocates.
//
//  The heap is **1-indexed**: slot 0 is never read or written, the root lives
//  at slot 1, and both loops stop the moment the index reaches 1.  That is
//  why Push starts from `m_count - 1` after growing (the slot the new entry
//  landed in) and why PopMin's "is there anything to sift" test is `< 2`
//  rather than `< 1`.
//
//  Its own TU: Push contains a loop, so the shipped build kept it out of line
//  at its two call sites, and PopMin calls SAStarHeap::EraseRange - putting
//  either next to its callee lets /O2 inline the pair together.
//============================================================================
#include "pathastar.h"

//----------------------------------------------------------------------------
//  0x6F498580 - push one entry, percolating it up to its place.
//
//  `entry` is taken **by value**: the shipped call site (CPathAStar::Reopen,
//  0x6F498B80) builds it with `sub esp,0Ch` and three stores through a copy
//  of `esp`, which is what MSVC emits for a by-value struct argument and not
//  what it emits for three separate pushes.
//----------------------------------------------------------------------------
void SAStarHeap::Push(SAStarHeapEntry entry)
{
    Grow(0, 1);

    unsigned int slot = m_count - 1;
    while (slot > 1)
    {
        SAStarHeapEntry* base = Entries();
        unsigned int parent = slot >> 1;

        if (entry.m_key > base[parent].m_key)
            break;

        base[slot] = base[parent];
        slot = parent;
    }

    Entries()[slot] = entry;
}

//----------------------------------------------------------------------------
//  0x6F4985F0 - pop the minimum into `*out` and sift the old last entry back
//  down.  Returns `out`, which is what the caller (CPathAStar::RunLoop) reads
//  the popped entry back out of.
//
//  The erase happens *before* the sift-down and the last entry is read out
//  first, so the loop below runs against the already-shortened array.
//----------------------------------------------------------------------------
SAStarHeapEntry* SAStarHeap::PopMin(SAStarHeapEntry* out)
{
    SAStarHeapEntry* base = Entries();
    *out = base[1];

    unsigned int count = m_count;
    SAStarHeapEntry moved = base[count - 1];

    if (count != 0)
        EraseRange(count - 1, 1);

    unsigned int live = m_count;
    if (live < 2)
        return out;

    unsigned int last = live - 1;
    unsigned int slot = 1;
    unsigned int limit = last >> 1;

    while (limit >= slot)
    {
        unsigned int child = slot * 2;
        if (child < last && Entries()[child + 1].m_key <= Entries()[child].m_key)
            child++;

        SAStarHeapEntry* pick = &Entries()[child];
        if (moved.m_key <= pick->m_key)
            break;

        Entries()[slot] = *pick;
        slot = child;

        if (child > limit)
            break;
    }

    Entries()[slot] = moved;
    return out;
}
