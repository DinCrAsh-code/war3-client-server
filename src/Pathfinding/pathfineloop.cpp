//============================================================================
//  0x6F4A2E00 - the fine search's main loop.
//
//  CPathAStar::RunLoop (pathastarloop.cpp) with two differences: the
//  expansion is one call rather than a type-keyed pair, and an expansion
//  that sets m_abort stops the search *without* closing the node it was
//  working on and hands that node back as the result.
//============================================================================
#include "pathfinesearch.h"

int CPathFineSearch::RunLoop()
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
        SAStarNode* n = &Nodes()[idx];
        if (gen == popped.m_gen)
        {
            n->m_prev = -1;
            n->m_next = -1;
            n->m_gen = gen + 1;

            if (idx == m_destNode)
                return idx;

            Expand(idx);

            //  Not a `break`: the shipped code's clear-the-flag arm falls
            //  straight into the same epilogue the goal-reached return uses,
            //  so an expansion that asked to stop returns *this* node, the
            //  one it was working on - the same answer reaching the goal
            //  gives, not the -1 that running out of budget gives.
            if (m_abort != 0)
            {
                m_abort = 0;
                return idx;
            }

            Close(idx);
        }
    }

    return result;
}
