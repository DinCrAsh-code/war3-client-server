//============================================================================
//  0x6F60BBC0 - CFramePriorityArray::Insert: put one entry in, keeping the
//  array sorted by descending priority.
//
//  The reserve grows eight at a time, and the tail is shifted up by one with
//  a descending loop rather than a memmove - one whose cursor is decremented
//  *before* the store, which is what forms the destination address with its
//  own `lea` (docs/msvc-vc8-idioms.md, "Advance the cursor before the test
//  that reads the old element").
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CFramePriorityArray::Insert(FRAMEPRIORITY* entry)
{
    unsigned int priority = entry->m_priority;

    unsigned int reserved = m_reserved;
    if (m_used == reserved)
    {
        reserved += 8;
        m_reserved = reserved;
        SetCount(reserved);
    }

    unsigned int count = m_used;
    unsigned int index = 0;

    if (count > 0)
    {
        FRAMEPRIORITY** cursor = m_data;

        do
        {
            FRAMEPRIORITY* at = *cursor;

            if (priority > at->m_priority)
            {
                while (count > index)
                {
                    FRAMEPRIORITY** data = m_data;
                    FRAMEPRIORITY*  moved = data[count - 1];
                    FRAMEPRIORITY** slot  = &data[count];
                    count -= 1;
                    *slot = moved;
                }

                m_data[index] = entry;
                break;
            }

            index += 1;
            cursor += 1;
        }
        while (index < count);
    }

    if (index == m_used)
        m_data[index] = entry;

    m_used += 1;
}
