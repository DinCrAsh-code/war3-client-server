//============================================================================
//  0x6F49A800 - the per-cell A* as its callers see it.
//
//  Empty the output cache, set the endpoints up, and then one of four things
//  happens:
//
//    * Setup says the query is trivial - both endpoints in the same cell, or
//      one of them has no node at all - and the destination is queued as the
//      only waypoint;
//    * the search reaches the goal, and the parent chain is walked back into
//      a route;
//    * it does not, and the best node it found is the one it started on, so
//      there is nowhere to go: the *start* is queued as the only waypoint;
//    * it does not, but the best node is somewhere else - so the goal point
//      is replaced with that node's cell centre and the route is built to
//      there instead.  That last one is why m_to is not const: an
//      unreachable destination is quietly retargeted at the closest thing
//      the budget could find.
//
//  Reaching the goal returns 1 and everything else returns 0, so the caller
//  can tell "this is the route you asked for" from "this is the best I have".
//============================================================================
#include "pathastar.h"
#include "pathfind.h"
#include "widgetfootprint.h"

int CPathAStar::RunSearch(int maskShift, SPathPointCache* out,
                          const CGridVec2* from, const CGridVec2* to,
                          int budget, int tier, int useCellLevel)
{
    if (out->m_count != 0)
        out->EraseRange(0, out->m_count);

    if (Setup(maskShift, from, to, budget, tier, useCellLevel) == 1)
    {
        QueuePoint(out, to);
        return 1;
    }

    int node = Begin();
    if (node != -1)
    {
        ((CPathSearch*)this)->BuildRoutePoints(node, out, from, to);
        return 1;
    }

    if (m_startNode == m_bestNode)
    {
        QueuePoint(out, from);
        return 0;
    }

    //  SFootprintPos, not a bare CFloat[2]: the zeroing default constructor
    //  on a two-element array turns into a `vector constructor iterator`
    //  call the shipped code does not have, and this is the pair type that
    //  already carries the kCFloatNoInit tag for exactly that.
    SFootprintPos centre;
    SAStarNode* best = &Nodes()[m_bestNode];
    CFloat* pt = m_grids[0]->CellCentre(&centre.m_x, (const SCellPoint*)best);

    //  &m_to bound once and used for both stores and the argument: the
    //  shipped code forms the address once and reaches the second field
    //  through it, not through `this` again.
    CGridVec2* retarget = &m_to;
    retarget->m_x = pt[0];
    retarget->m_y = pt[1];

    ((CPathSearch*)this)->BuildRoutePoints(m_bestNode, out, from,
                                          retarget);
    return 0;
}
