//============================================================================
//  0x6F49A3F0 - empty the per-cell A* back to a fresh search.
//
//  CPathFineSearch::Reset (pathfinereset.cpp) with one extra counter cleared;
//  see that file for why the node array gets a pointer of its own and the
//  heap does not, and why the heap is grown by one straight away (slot 0 of a
//  1-indexed heap is never touched, so it has to exist before the first push
//  rather than being made by it).
//============================================================================
#include "pathastar.h"

void CPathAStar::Reset(int shrink)
{
    SAStarNodeArray* nodes = &m_nodes;

    m_expansions = 0;
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
