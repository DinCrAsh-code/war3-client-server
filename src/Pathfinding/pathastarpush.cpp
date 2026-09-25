//============================================================================
//  0x6F49A240 - score the destination node and start the search.
//
//  The heuristic is a straight Euclidean distance between the start cell and
//  the destination cell, both already quantised, with each axis scaled by 24
//  first - the same 24 units per cell every cost in this search is measured
//  in, so h comes out directly comparable to the accumulated g the relax step
//  builds out of edge lengths.
//
//  The node scored and pushed is the *start* node, not the destination: this
//  is the seeding step, and RunLoop stops when m_destNode comes back off the
//  heap.  Only h is written - g is whatever is already in the node, which for
//  the start node is the zero Reset left there - and Reopen pushes f = g + h.
//
//  The tail call into RunLoop is the search itself; the shipped code really
//  does jump rather than call, so this function's own frame is gone before
//  the loop starts.
//============================================================================
#include "pathastar.h"

int CPathAStar::Begin()
{
    SAStarNode* start = &Nodes()[m_startNode];

    //  Both axes are reduced to an absolute difference first and only then
    //  scaled - same ordering CPathAStar::Relax needs, same reason.
    int dx = m_startX - m_destX;
    if (dx < 0)
        dx = -dx;

    int dy = m_startY - m_destY;
    if (dy < 0)
        dy = -dy;

    dx *= 24;
    dy *= 24;

    start->m_h = IntegerSqrtNewton(dy * dy + dx * dx);

    Reopen(m_startNode);
    return RunLoop();
}
