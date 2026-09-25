//============================================================================
//  0x6F493810 - splice one order into the list, immediately after `this`.
//
//  A plain doubly-linked insert, with 0 rather than a back-pointer to the
//  anchor as the end marker: the tail's m_next is null, so the "fix the
//  successor's back pointer" step is conditional.
//============================================================================
#include "pathorder.h"

void SPathOrderLink::InsertAfter(SPathOrderLink* node)
{
    node->m_prev = this;
    node->m_next = m_next;

    if (m_next != 0)
        m_next->m_prev = node;

    m_next = node;
}
