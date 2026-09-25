//============================================================================
//  0x6F49A170 - the A* main loop: pop the cheapest open node, drop it if the
//  generation stamp says a better entry for it has since been pushed, expand
//  it, close it, repeat.
//
//  Two things are worth pointing out because they read oddly in source form.
//
//  `result` is initialised to -1 and never assigned again - the only way this
//  function returns anything else is the early `return popped.m_node` when the
//  goal comes off the heap.  It is a variable rather than a literal because
//  the shipped code parks -1 in ebp across the whole function and returns it
//  from two different exits.
//
//  And the budget test spends its iteration whether or not it passes:
//  `m_iterations++ >= m_budget` increments unconditionally and compares the
//  value from *before* the increment, which is exactly the `lea ecx,[eax+1]` /
//  `cmp eax,m_budget` / `mov m_iterations,ecx` order the shipped code has.
//============================================================================
#include "pathastar.h"

int CPathAStar::RunLoop()
{
    int result = -1;

    while (m_heap.m_count > 1)
    {
        if (m_iterations++ >= m_budget)
            break;

        SAStarHeapEntry popped;
        m_heap.PopMin(&popped);

        int idx = (int)popped.m_node;

        unsigned int gen = Nodes()[idx].m_gen;
        SAStarNode* popped_node = &Nodes()[idx];
        if (gen == popped.m_gen)
        {
            popped_node->m_prev = -1;
            popped_node->m_next = -1;
            popped_node->m_gen = gen + 1;

            if (idx == m_destNode)
                return idx;

            int x = Nodes()[idx].m_x;
            int y = Nodes()[idx].m_y;
            SAStarNode* n = &Nodes()[idx];
            int type = n->m_type;

            if (type == 0)
                ExpandCell(idx, x, y, n->m_linkSlot);
            else
                ExpandPortal(idx, type, x, y);

            Close(idx);
        }
    }

    return result;
}
