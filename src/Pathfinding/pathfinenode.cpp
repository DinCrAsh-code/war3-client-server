//============================================================================
//  0x6F4A18D0 / 0x6F4A1920 - the fine search's open/closed bookkeeping, the
//  same pair CPathAStar::Reopen/Close (pathastarnode.cpp) are for the other
//  instance, reading its arrays at +0x24/+0x44 instead of +0x50/+0x70.
//============================================================================
#include "pathfinesearch.h"

void CPathFineSearch::Reopen(int node)
{
    SAStarNode* n = &Nodes()[node];

    n->m_gen++;

    SAStarHeapEntry entry;
    entry.m_key = n->m_h + n->m_g;
    entry.m_node = node;
    entry.m_gen = n->m_gen;

    n->m_prev = -2;
    n->m_next = -1;

    m_heap.Push(entry);
}

void CPathFineSearch::Close(int node)
{
    int tail = m_closedTail;
    if (tail != -3)
        Nodes()[tail].m_next = node;

    SAStarNode* n = &Nodes()[node];
    int prev = m_closedTail;
    n->m_gen++;
    n->m_prev = prev;
    n->m_next = -1;

    m_closedTail = node;
}
