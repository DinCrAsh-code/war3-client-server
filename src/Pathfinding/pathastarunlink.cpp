//============================================================================
//  0x6F498C20 / 0x6F498D90 - taking a node off the closed list, and the link
//  neighbour's own relax.
//============================================================================
#include "pathastar.h"

//----------------------------------------------------------------------------
//  0x6F498C20 - splice `node` out of the closed list and bump its generation
//  so that any heap entry still naming it reads as stale.
//
//  -1 is the list's own tail marker and -3 is "the list is empty", which is
//  why the two ends are tested against different sentinels: a node whose
//  m_next is -1 was the tail, and a m_prev of -3 means there is nothing in
//  front of it.
//----------------------------------------------------------------------------
void CPathAStar::Unlink(int node)
{
    SAStarNode* n = &Nodes()[node];

    n->m_gen++;

    int prev = n->m_prev;
    int next = n->m_next;

    n->m_prev = -1;
    n->m_next = -1;

    if (next != -1)
        Nodes()[next].m_prev = prev;
    else
        m_closedTail = prev;

    if (prev != -3)
        Nodes()[prev].m_next = next;
}

//----------------------------------------------------------------------------
//  0x6F498D90 - CPathAStar::Relax (pathastarrelax.cpp) for a step across the
//  link table rather than across the grid.
//
//  Two differences, both because a link is not a distance: the edge costs a
//  flat 1 instead of the Euclidean length between the two cells, and the
//  node inherits the parent's link slot instead of having its own cleared.
//  Everything after that - the improvement test, the closed/reopen handling,
//  the closest-to-goal fallback, the heuristic - is the same.
//----------------------------------------------------------------------------
void CPathAStar::RelaxLink(int node, int parent)
{
    SAStarNode* n = &Nodes()[node];
    SAStarNode* p = &Nodes()[parent];

    unsigned int g = p->m_g + 1;

    //  Both axes reduced to an absolute difference before either is scaled,
    //  same as CPathAStar::Relax and for the same reason: doing one axis end
    //  to end costs the register the shipped code keeps the x difference in.
    //  What is left over is four instructions of the x scaling landing after
    //  the y subtraction rather than before it, which no spelling tried here
    //  moves without giving the registers back up.
    int hx = n->m_x - m_destX;
    if (hx < 0)
        hx = -hx;

    int hy = n->m_y - m_destY;
    if (hy < 0)
        hy = -hy;

    hx *= 24;
    hy *= 24;

    unsigned int h = IntegerSqrtNewton(hy * hy + hx * hx);

    if (n->m_prev != -1)
    {
        if (g >= n->m_g)
            return;

        //  Written the other way round from CPathAStar::Relax's identical
        //  test: this one's shipped code puts the unlink call on the
        //  fall-through and the generation bump out of line.
        if ((unsigned int)n->m_prev < 0xFFFFFFFE)
            Unlink(node);
        else
        {
            n->m_gen++;
            n->m_prev = -1;
            n->m_next = -1;
        }
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
    n->m_flag = p->m_linkSlot;
    n->m_g = g;
    n->m_h = h;

    Reopen(node);
}
