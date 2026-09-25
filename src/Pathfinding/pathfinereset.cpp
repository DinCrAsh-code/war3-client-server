//============================================================================
//  0x6F4A2DA0 - empty the fine search back to a fresh state.
//
//  Both arrays are erased from 0 rather than having their counts zeroed, so
//  that CMemBlock keeps the allocation for the next search; `shrink` is what
//  hands it back instead.  The heap is then grown by one straight away - slot
//  0 of a 1-indexed heap is never read or written, so it has to exist before
//  the first push rather than being created by it.
//============================================================================
#include "pathfinesearch.h"

void CPathFineSearch::Reset(int shrink)
{
    //  The node array gets a pointer of its own and the heap does not: the
    //  shipped code holds &m_nodes in one callee-saved register for the whole
    //  body and re-points its own `this` register at &m_heap half way down,
    //  so only one extra register is ever live.  Naming both costs a third.
    SAStarNodeArray* nodes = &m_nodes;

    m_iterations = 0;
    m_closedTail = -3;

    if (nodes->m_count != 0)
        nodes->EraseRange(0, nodes->m_count);

    unsigned int queued = m_heap.m_count;
    SAStarHeap* heap = &m_heap;
    if (queued != 0)
        heap->EraseRange(0, queued);

    heap->Grow(0, 1);

    if (shrink)
    {
        nodes->ShrinkToFit();
        heap->ShrinkToFit();
    }
}
