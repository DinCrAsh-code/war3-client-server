//============================================================================
//  0x6F60BAF0 - CFramePriorityArray::SetCount: make room for `count`
//  elements, rounded up to a whole chunk, and remember the request.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CFramePriorityArray::SetCount(unsigned int count)
{
    if (count > m_count && count > m_alloc)
    {
        unsigned int chunk = m_chunk;
        if (chunk == 0)
            chunk = ComputeChunk(count);

        unsigned int alloc = count;
        unsigned int over = count % chunk;
        if (over != 0)
            alloc = chunk - over + count;

        SetAlloc(alloc);
    }

    m_count = count;
}
