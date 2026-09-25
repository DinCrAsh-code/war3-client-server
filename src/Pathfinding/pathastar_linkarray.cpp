//============================================================================
//  0x6F498100 / 0x6F498180 / 0x6F497780 - SAStarLinkArray's own storage
//  (pathastar.h's own header comment on the class - the same object
//  CPathAStar's `m_links` field already points into, at its own three
//  addresses because this image was linked without /OPT:ICF - the identical
//  template already instantiated for SAStarHeap (pathastarheaparray.cpp)
//  gets a second, separate set of real functions here rather than sharing
//  the first set's.
//============================================================================
#include "pathastar.h"

extern "C" void* __cdecl memmove(void* dst, const void* src, unsigned int count);

//----------------------------------------------------------------------------
//  0x6F497780 - the repeated-value fill Grow runs over the slots it just
//  added.  Same shape as SAStarHeap::FillFrom.
//----------------------------------------------------------------------------
void SAStarLinkArray::FillFrom(unsigned int start, const SAStarLink* value,
                                unsigned int count)
{
    unsigned int end = start + count;
    if (end >= m_count)
        end = m_count;

    if (start < end)
        for (unsigned int i = start; i < end; i++)
            Entries()[i] = *value;
}

//----------------------------------------------------------------------------
//  0x6F498100 - same shape as SAStarHeap::Grow.
//----------------------------------------------------------------------------
int SAStarLinkArray::Grow(const SAStarLink* fill, unsigned int count)
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
//  0x6F498180 - shift-erase [start, start+count) out of the array.  Same
//  shape as SAStarHeap::EraseRange.
//----------------------------------------------------------------------------
int SAStarLinkArray::EraseRange(unsigned int start, unsigned int count)
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
