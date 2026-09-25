//============================================================================
//  0x6F4A19B0 - the fine search's relax step.
//
//  Same shape as CPathAStar::Relax (pathastarrelax.cpp) - improve or leave
//  alone, splice off the closed list, track the closest node to the goal,
//  re-push - with two differences, both about how distance is measured.
//
//  The edge cost is not computed at all: CPathFineSearch::Expand hands it in,
//  15 for a cardinal step and 21 for a diagonal.
//
//  And the heuristic is an alpha-max-plus-beta-min approximation of the
//  hypotenuse rather than a real square root - no call, no isqrt, six shifts
//  and four adds:
//
//      h ~= 0.953125 * max + 0.38671875 * min
//
//  with both axes pre-scaled by the same 15 the cardinal step costs, and an
//  early exit straight to `max` once the shorter axis is inside a quarter of
//  the longer one.  Worst case it is about 7% high against the true distance
//  over the cell ranges this search works in - it over-estimates, which for
//  an A* heuristic costs optimality but never terminates early.
//
//  The shifts really are what they look like: 1 + 1/64 - 1/16 for the long
//  axis, and 3/8 + 3/256 of the short one reached as `min + min/2` shifted
//  twice.
//============================================================================
#include "pathfinesearch.h"

void CPathFineSearch::Relax(int node, int parent, int cost)
{
    int nx = Nodes()[node].m_x;
    SAStarNode* n = &Nodes()[node];

    unsigned int g = Nodes()[parent].m_g + cost;

    int ax = nx - m_destX;
    if (ax < 0)
        ax = -ax;

    int ny = n->m_y;
    unsigned int wide = (unsigned int)(ax * 15);

    int ay = ny - m_destY;
    if (ay < 0)
        ay = -ay;

    unsigned int tall = (unsigned int)(ay * 15);

    //  `lo` starts as the y axis and is only replaced when the x axis turns
    //  out to be the shorter of the two - which is what leaves it already in
    //  place on the other arm, the way the shipped code has it.
    unsigned int lo = tall;
    unsigned int hi;
    if (wide >= tall)
        hi = wide;
    else
    {
        hi = tall;
        lo = wide;
    }

    unsigned int h = hi;
    if ((hi >> 2) < lo)
    {
        unsigned int half = lo + (lo >> 1);
        h = (hi >> 6) + ((half >> 2) + hi + ((half >> 7) - (hi >> 4)));
    }

    if (n->m_prev != -1)
    {
        if (g >= n->m_g)
            return;

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
        int dy = ny - m_destY;
        int dx = nx - m_destX;
        unsigned int distSq = dx * dx + dy * dy;

        if (distSq < m_bestDistSq)
        {
            m_bestDistSq = distSq;
            m_bestNode = node;
        }
    }

    n->m_parent = parent;
    n->m_g = g;
    n->m_h = h;

    Reopen(node);
}
