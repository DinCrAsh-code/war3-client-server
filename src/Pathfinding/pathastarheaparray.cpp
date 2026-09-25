//============================================================================
//  0x6F4865B0 / 0x6F498060 / 0x6F497CC0 - the A* open list's own storage.
//
//  The same CMemBlock-backed growable array SPathFrontier (4-byte element,
//  pathfrontierarray.cpp) and SPathPointCache (8-byte, pathpointcachegrow.cpp)
//  are, instantiated here for the 12-byte SAStarHeapEntry.  Every index
//  computation is MSVC's canonical *12: `lea reg,[i+i*2]` then two `add
//  reg,reg`.
//============================================================================
#include "pathastar.h"

extern "C" void* __cdecl memmove(void* dst, const void* src, unsigned int count);


//----------------------------------------------------------------------------
//  0x6F483FF0 - the repeated-value fill Grow runs over the slots it just added.
//----------------------------------------------------------------------------
void SAStarHeap::FillFrom(unsigned int start, const SAStarHeapEntry* value,
                     unsigned int count)
{
    unsigned int end = start + count;
    if (end >= m_count)
        end = m_count;

    if (start < end)
        for (unsigned int i = start; i < end; i++)
            Entries()[i] = *value;
}

int SAStarHeap::Grow(const SAStarHeapEntry* fill, unsigned int count)
{
    if (count == 0)
        return count + 1;

    unsigned int needed = m_count + count;
    if (needed > m_capacity)
    {
        unsigned int chunk = m_growChunk;
        if (chunk == 0)
            return 0;

        unsigned int shortfall = m_count - m_capacity + count;
        unsigned int grow = (chunk > shortfall) ? chunk : shortfall;

        if (!SetSize((m_capacity + grow) * 12, 1))
            return 0;

        m_capacity += grow;
    }

    unsigned int start = m_count;
    m_count = start + count;
    if (fill != 0)
        FillFrom(start, fill, m_count);
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F498060 - shift-erase [start, start+count) out of the heap array.
//----------------------------------------------------------------------------
int SAStarHeap::EraseRange(unsigned int start, unsigned int count)
{
    unsigned int total = m_count;
    if (start >= total)
        return 0;

    if (start + count > total)
        count = total - start;

    unsigned int trailing = total - start - count;
    if (trailing != 0)
        memmove(Entries() + start, Entries() + start + count, trailing * 12);

    m_count -= count;
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F497CC0 - shrink the allocation down to exactly the live entries and
//  report how many bytes that gave back.  m_size is CMemBlock's own record of
//  the usable size, so the difference is read across the SetSize call rather
//  than computed from the counts.
//----------------------------------------------------------------------------
int SAStarHeap::ShrinkToFit()
{
    unsigned int before = m_size;
    SetSize(m_count * 12, 1);
    m_capacity = m_count;
    return before - m_size;
}
