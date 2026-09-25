//============================================================================
//  0x6F47D480 / 0x6F46AC70 - AgileAgentQuery's own array primitives: fill a
//  run of entries from one value, and shift-erase a run out.
//
//  Both are the eight-byte-element siblings of what CCellRecordArray
//  (spatialgrid.cpp) and SPathFrontier (pathfrontierarray.cpp) already
//  carry for their own element sizes, and they are written the same way -
//  including FillEntries' re-reading of m_data inside the loop, which is
//  what the shipped stream does because `value` may alias the block.
//
//  One translation unit, and not the one AppendEntries is in: that function
//  calls FillEntries for real.
//============================================================================
#include "agentquery.h"

extern "C" void* __cdecl memmove(void* dst, const void* src, unsigned int count);

//----------------------------------------------------------------------------
//  0x6F47D480
//----------------------------------------------------------------------------
void AgileAgentQuery::FillEntries(unsigned int start,
                                  const SAgentQueryHit* value,
                                  unsigned int count)
{
    //  Not `end = start + count` clamped down to m_count: the shipped code
    //  starts from m_count and only takes the sum when the sum is smaller,
    //  which is the other operand order and the other branch sense.
    unsigned int end = m_count;
    if (start + count < end)
        end = start + count;

    if (start < end)
        for (unsigned int i = start; i < end; i++)
            ((SAgentQueryHit*)m_data)[i] = *value;
}

//----------------------------------------------------------------------------
//  0x6F46AC70
//----------------------------------------------------------------------------
int AgileAgentQuery::RemoveEntries(unsigned int start, unsigned int count)
{
    unsigned int total = m_count;
    if (start >= total)
        return 0;

    if (start + count > total)
        count = total - start;

    unsigned int trailing = total - start - count;
    if (trailing != 0)
        memmove((SAgentQueryHit*)m_data + start,
                (SAgentQueryHit*)m_data + start + count, trailing * 8);

    m_count -= count;
    return 1;
}
