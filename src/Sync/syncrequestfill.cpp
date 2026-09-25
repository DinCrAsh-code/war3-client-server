//============================================================================
//  0x6F468570 - SSyncRequestHeap::FillFrom: write one repeated entry over
//  [start, min(start + count, m_count)).
//
//  The same body SPathFrontier::FillFrom (pathfrontierarray.cpp) has, for
//  the same 4-byte pointer element and with the same two things worth
//  saying: the source is a single value repeated and not an array (the
//  shipped loop never advances it, and re-reads it through the pointer on
//  every iteration because nothing tells the compiler the store cannot
//  alias it), and the end is clamped down to m_count rather than trusted,
//  which is what lets Grow() hand it the new count instead of the added
//  count.
//
//  Own translation unit: Grow() calls it for real.
//============================================================================
#include "syncrequest.h"

void SSyncRequestHeap::FillFrom(unsigned int start, const void* value,
                                unsigned int count)
{
    unsigned int end = start + count;
    if (end >= m_count)
        end = m_count;

    if (start < end)
        for (unsigned int i = start; i < end; i++)
            Entries()[i] = *(SSyncRegistration* const*)value;
}
