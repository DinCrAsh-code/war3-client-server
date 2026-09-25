//============================================================================
//  0x6F4A2EA0 - give the start node its heuristic, push it, and run the loop.
//
//  CPathAStar::Begin (pathastarpush.cpp) for the fine search: the same
//  seeding step, with the alpha-max-plus-beta-min distance
//  CPathFineSearch::Relax uses (pathfinerelax.cpp) in place of the integer
//  square root.  Only h is written; g is the zero Reset left in the node.
//
//  The tail call into RunLoop is the search - the shipped code jumps rather
//  than calls, so this frame is gone before the loop starts.
//============================================================================
#include "pathfinesearch.h"

int CPathFineSearch::Begin()
{
    SAStarNode* start = &Nodes()[m_startNode];

    int ax = m_startX - m_destX;
    if (ax < 0)
        ax = -ax;

    unsigned int wide = (unsigned int)(ax * 15);

    int ay = m_startY - m_destY;
    if (ay < 0)
        ay = -ay;

    unsigned int tall = (unsigned int)(ay * 15);

    //  Spelled with the shorter-x case first: this one's shipped code has
    //  the swap on the fall-through and `hi = wide` out of line, where
    //  CPathFineSearch::Relax's copy of the same three lines has it the
    //  other way round.
    unsigned int lo = tall;
    unsigned int hi;
    if (wide < tall)
    {
        hi = tall;
        lo = wide;
    }
    else
        hi = wide;

    unsigned int h = hi;
    if ((hi >> 2) < lo)
    {
        unsigned int half = lo + (lo >> 1);
        h = (hi >> 6) + ((half >> 2) + hi + ((half >> 7) - (hi >> 4)));
    }

    start->m_h = h;

    Reopen(m_startNode);
    return RunLoop();
}
