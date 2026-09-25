//============================================================================
//  0x6F4A0C90 / 0x6F4A0BA0 - CPathSearch's own frontier: grow by `count`
//  slots, then fill every one of them from `*value` (a single pointer-sized
//  value repeated, not an array - the shipped loop never advances the
//  source).  Same shape as CCellRecordArray::AppendRecords/FillRecords
//  (spatialgrid.cpp), just for a 4-byte pointer element.
//============================================================================
#include "pathfind.h"

extern "C" void* __cdecl memmove(void* dst, const void* src, unsigned int count);

void SPathFrontier::FillFrom(unsigned int start, const void* value,
                             unsigned int count)
{
    unsigned int end = start + count;
    if (end >= m_count)
        end = m_count;

    if (start < end)
        for (unsigned int i = start; i < end; i++)
            Nodes()[i] = *(void* const*)value;
}

int SPathFrontier::Grow(const void* fill, unsigned int count)
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

        if (!SetSize((m_capacity + grow) * 4, 1))
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
//  0x6F49B6F0 - shift-erase [start, start+count) out of the frontier.
//----------------------------------------------------------------------------
int SPathFrontier::EraseRange(unsigned int start, unsigned int count)
{
    unsigned int total = m_count;
    if (start >= total)
        return 0;

    if (start + count > total)
        count = total - start;

    unsigned int trailing = total - start - count;
    if (trailing != 0)
        memmove(Nodes() + start, Nodes() + start + count, trailing * 4);

    m_count -= count;
    return 1;
}
