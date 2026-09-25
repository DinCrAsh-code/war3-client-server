//============================================================================
//  0x6F4A3CC0 - the fine per-cell search as its callers see it.
//
//  CPathAStar::RunSearch (pathastarrun.cpp) on the other context, with the
//  same four outcomes: trivial, reached, nowhere to go, or retargeted at the
//  closest node the budget found.  See that file for what each one means.
//
//  Two of its own arguments reach Setup in the other order from the one they
//  arrive in - that is what the shipped push sequence does, not a slip here.
//============================================================================
#include "pathfinesearch.h"
#include "widgetfootprint.h"

int CPathFineSearch::RunSearch(SPathPointCache* out, const CGridVec2* from,
                               const CGridVec2* to, const int* mask,
                               int budget, const CFloat* radius,
                               int typeFlags)
{
    if (out->m_count != 0)
        out->EraseRange(0, out->m_count);

    if (Setup(from, to, budget, mask, radius, typeFlags) == 1)
    {
        QueuePoint(out, to);
        return 1;
    }

    int node = Begin();
    if (node != -1)
    {
        BuildRoute(node, out, from, to);
        return 1;
    }

    if (m_startNode == m_bestNode)
    {
        QueuePoint(out, from);
        return 0;
    }

    //  The best node's address into a local of its own before the call:
    //  the shipped code forms it, then loads the grid into ecx, then
    //  pushes, and leaving the subscript in the argument defers the
    //  receiver load past both pushes and swaps two registers with it.
    //  Same three instructions as CPathAStar::RunSearch's own copy.
    SFootprintPos centre;
    SAStarNode* best = &Nodes()[m_bestNode];
    CFloat* pt = m_grid->CellCentre(&centre.m_x, (const SCellPoint*)best);

    CGridVec2* retarget = &m_to;
    retarget->m_x = pt[0];
    retarget->m_y = pt[1];

    BuildRoute(m_bestNode, out, from, retarget);
    return 0;
}
