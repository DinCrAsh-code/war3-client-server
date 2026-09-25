//============================================================================
//  0x6F498B80 / 0x6F498BD0 - the two node-list operations the main loop runs
//  around every expansion.
//
//  Together they are the whole of this A*'s open/closed bookkeeping.  There
//  is no decrease-key: SAStarHeap has no index map, so a node whose g
//  improves is re-pushed with a fresh generation stamp and the entry already
//  on the heap is left to be recognised as stale when it is popped.  m_prev /
//  m_next thread the closed list through the nodes themselves, with
//  m_closedTail as its tail and -3 as the "list is empty" sentinel.
//============================================================================
#include "pathastar.h"

//----------------------------------------------------------------------------
//  0x6F498B80 - (re)open `node`: bump its generation so that any entry still
//  sitting on the heap for it reads as stale, then push its current f = g + h.
//----------------------------------------------------------------------------
void CPathAStar::Reopen(int node)
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

//----------------------------------------------------------------------------
//  0x6F498BD0 - append `node` to the closed list.
//
//  The old tail's m_next is only written when there *is* an old tail: -3, not
//  -1, is what m_closedTail holds while the list is empty, because -1 and -2
//  are already spoken for as a node's own "not on the list" / "was closed"
//  markers in m_prev.
//----------------------------------------------------------------------------
void CPathAStar::Close(int node)
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
