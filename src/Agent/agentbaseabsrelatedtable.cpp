//============================================================================
//  0x6F4A4FE0 / 0x6F4A5060 - the two `CRelatedAgentsTable` operations that
//  are reached with `&m_relatedAgents` itself in `ecx`, rather than through
//  the outer agent's `+0x64`/`+0x74` window. See agentbaseabs_related.h for
//  the field-offset evidence this file's own accesses helped establish.
//============================================================================
#include "agentbaseabs_related.h"
#include "cellbuffer.h"

extern "C" void* __cdecl memmove(void* dst, const void* src, unsigned int count);

//----------------------------------------------------------------------------
//  0x6F4A4FE0 - grow the table if needed, then append `count` pointer-sized
//  elements from `*items` (nullable). See this file's header/agentbaseabs_
//  related.h for the growth-gate ("no growth hint means fixed capacity")
//  reasoning.
//----------------------------------------------------------------------------
int CRelatedAgentsTable::GrowInsert(const void* items, unsigned int count)
{
    if (count == 0)
        return count + 1;

    unsigned int capacity = m_capacity;
    unsigned int oldCount = m_count;
    if (oldCount + count > capacity)
    {
        unsigned int growthHint = m_growthHint;
        if (growthHint == 0)
            return 0;

        unsigned int deficit = oldCount - capacity + count;
        unsigned int delta = (growthHint > deficit) ? growthHint : deficit;

        if (!((CMemBlock*)this)->SetSize((capacity + delta) * 4, 1))
            return 0;

        m_capacity += delta;
    }

    //  Re-read m_count fresh: the shipped code does too, since SetSize is an
    //  out-of-line call the compiler cannot prove leaves it alone.
    unsigned int newCount = m_count + count;
    const void* src = items;
    m_count = newCount;
    if (src != 0)
        ((CRelatedAgentsTableInsertExternal*)this)
            ->InsertAt_6F4A4AD0(oldCount, src, newCount);

    return 1;
}

//----------------------------------------------------------------------------
//  0x6F4A5060 - remove up to `count` elements at `index`, sliding the tail
//  down and shrinking `m_count`.
//----------------------------------------------------------------------------
int CRelatedAgentsTable::RemoveRange(unsigned int index, unsigned int count)
{
    unsigned int total = m_count;
    if (index >= total)
        return 0;

    unsigned int removeCount = count;
    if (index + removeCount > total)
        removeCount = total - index;

    unsigned int tailCount = total - index - removeCount;
    if (tailCount != 0)
    {
        char* data = (char*)m_data;
        memmove(data + index * 4, data + (index + removeCount) * 4,
                tailCount * 4);
    }

    m_count -= removeCount;
    return 1;
}
