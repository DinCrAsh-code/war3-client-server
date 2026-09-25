//============================================================================
//  0x6F498C70 - the relax step: this is the part that makes the search an A*
//  rather than a greedy walk.
//
//  Both distances are Euclidean and both go through the same integer square
//  root, with each axis scaled by 24 first - 24 units to the cell, which is
//  what makes an edge length and the heuristic directly comparable:
//
//      g = parent's g + |edge from parent to this node|
//      h = |this node to the destination cell|
//
//  A node that has never been reached (m_prev still -1) is accepted
//  unconditionally.  One that has is only re-parented when the new g is
//  strictly better, and how it comes off the open structures depends on where
//  it currently sits: m_prev == -2 means it was closed, so bumping its
//  generation is enough to make the heap entry still naming it read as stale;
//  anything else is a live closed-list position and has to be spliced out.
//
//  The closest-node-to-goal fallback is only tracked on the never-reached
//  arm, and it is tracked in *unscaled* cell units squared - no 24, no square
//  root - because nothing ever compares it against a g or an h, only against
//  other copies of itself.
//============================================================================
#include "pathastar.h"

void CPathAStar::Relax(int node, int parent)
{
    SAStarNode* n = &Nodes()[node];
    SAStarNode* p = &Nodes()[parent];

    unsigned int parentG = p->m_g;

    //  Both axes are reduced to an absolute difference first and only then
    //  scaled: doing each axis end to end instead costs the register the
    //  shipped code keeps the x difference in.
    int ex = p->m_x - n->m_x;
    if (ex < 0)
        ex = -ex;

    int ey = p->m_y - n->m_y;
    if (ey < 0)
        ey = -ey;

    ex *= 24;
    ey *= 24;

    unsigned int g = IntegerSqrtNewton(ey * ey + ex * ex) + parentG;

    int hx = n->m_x - m_destX;
    if (hx < 0)
        hx = -hx;
    hx *= 24;

    int hy = n->m_y - m_destY;
    if (hy < 0)
        hy = -hy;
    hy *= 24;

    unsigned int h = IntegerSqrtNewton(hy * hy + hx * hx);

    if (n->m_prev != -1)
    {
        if (g >= n->m_g)
            return;

        if ((unsigned int)n->m_prev >= 0xFFFFFFFE)
        {
            //  Closed, but not on the list any more: a fresh generation is
            //  all it takes to invalidate whatever is still on the heap.
            n->m_gen++;
            n->m_prev = -1;
            n->m_next = -1;
        }
        else
            Unlink(node);
    }
    else
    {
        int dy = n->m_y - m_destY;
        int dx = n->m_x - m_destX;
        unsigned int distSq = dx * dx + dy * dy;

        if (distSq < m_bestDistSq)
        {
            m_bestDistSq = distSq;
            m_bestNode = node;
        }
    }

    n->m_parent = parent;
    n->m_flag = 0;
    n->m_g = g;
    n->m_h = h;

    Reopen(node);
}
