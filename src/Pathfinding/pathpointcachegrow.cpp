//============================================================================
//  0x6F498770 / 0x6F4981F0 - SPathPointCache::Grow/FillFrom: grow by
//  `count` elements, then fill every new one from `*fill` (a single
//  CGridVec2 repeated, not an array - the shipped loop never advances the
//  source). Same shape as SPathFrontier::Grow/FillFrom
//  (pathfrontierarray.cpp), just an 8-byte element instead of a 4-byte
//  pointer.
//============================================================================
#include "pathmove.h"

extern "C" void* __cdecl memmove(void* dst, const void* src, unsigned int count);

void SPathPointCache::FillFrom(unsigned int start, const CGridVec2* value,
                               unsigned int count)
{
    //  m_count first, then the sum compared down against it - the shipped
    //  code loads the count before it forms `start + count`, which is what
    //  puts them in these registers and gives the `jb` this polarity.  Same
    //  correction SAStarNodeArray::FillFrom (pathastarnodearray.cpp) needed.
    unsigned int end = m_count;
    unsigned int want = start + count;
    if (want < end)
        end = want;

    if (start < end)
        for (unsigned int i = start; i < end; i++)
            Points()[i] = *value;
}

int SPathPointCache::Grow(const CGridVec2* fill, unsigned int count)
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

        if (!SetSize((m_capacity + grow) * 8, 1))
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
//  0x6F4987F0 - shift-erase [start, start+count) out of the array. Same
//  shape as SPathFrontier::EraseRange (pathfrontierarray.cpp), an 8-byte
//  element instead of a 4-byte pointer.
//----------------------------------------------------------------------------
int SPathPointCache::EraseRange(unsigned int start, unsigned int count)
{
    unsigned int total = m_count;
    if (start >= total)
        return 0;

    if (start + count > total)
        count = total - start;

    unsigned int trailing = total - start - count;
    if (trailing != 0)
        memmove(Points() + start, Points() + start + count, trailing * 8);

    m_count -= count;
    return 1;
}
