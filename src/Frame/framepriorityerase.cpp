//============================================================================
//  0x6F60BC50 - CFramePriorityArray::EraseAt: shift the tail down one and
//  give a whole chunk of reserve back once the slack passes eight.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CFramePriorityArray::EraseAt(unsigned int index)
{
    while (index < m_used - 1)
    {
        FRAMEPRIORITY** slot = m_data + index;
        slot[0] = slot[1];
        index += 1;
    }

    m_used -= 1;

    unsigned int reserved = m_reserved;
    if (reserved - m_used > 8)
    {
        unsigned int wanted = reserved - 8;
        m_reserved = wanted;

        if (wanted > m_count && wanted > m_alloc)
        {
            unsigned int chunk = m_chunk;
            if (chunk == 0)
                chunk = ComputeChunk(wanted);

            unsigned int alloc = wanted;
            unsigned int over = wanted % chunk;
            if (over != 0)
                alloc = chunk - over + wanted;

            SetAlloc(alloc);
        }

        m_count = wanted;
    }
}
