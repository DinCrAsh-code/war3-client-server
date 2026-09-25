//============================================================================
//  0x6F49A640 - set the search up and classify the query.
//
//  Reset, latch the caller's two endpoints and the mask shift, quantise both
//  to cells, and answer one of three things:
//
//      0   there is nothing to search - the start cell is off the grid;
//      1   there is nothing to search *for* - both endpoints are in the same
//          cell, or one of them has no node, or they resolve to the same
//          node;
//      2   a real search is needed, and the fallback best-so-far has been
//          seeded with the start node and its own distance to the goal.
//
//  m_mode is `1 << tier`, not the tier itself, which is why the passability
//  gates downstream test it against 2 rather than 1.
//
//  The two CFloat temporaries the four floors run through land in this
//  function's own argument slots - MSVC reusing parameters it has finished
//  with - so the frame carries no locals of its own.
//============================================================================
#include "pathastar.h"
#include "boxmath.h"

int CPathAStar::Setup(int maskShift, const CGridVec2* from,
                      const CGridVec2* to, int budget, int tier,
                      int useCellLevel)
{
    Reset(0);

    m_useCellLevel = useCellLevel;
    m_maskShift = maskShift;

    CGridVec2* start = &m_from;
    CGridVec2* goal = &m_to;

    *start = *from;
    *goal = *to;

    m_mode = 1 << tier;

    int startY = CFloatToInt(CFloatFloor(start->m_y));
    int startX = CFloatToInt(CFloatFloor(start->m_x));
    m_startX = startX;
    m_startY = startY;

    int destY = CFloatToInt(CFloatFloor(goal->m_y));
    m_destX = CFloatToInt(CFloatFloor(goal->m_x));
    m_destY = destY;

    if (m_grids[0]->CellAt(m_startX, m_startY) == 0)
        return 0;

    if (m_startX != m_destX || m_startY != m_destY)
    {
        m_searchStamp++;

        //  Early return rather than an `else`: the shipped code has this
        //  store and its `return 1` inline, immediately after the test, and
        //  wrapping it in an else puts the whole thing out of line past the
        //  second lookup.
        m_startNode = LookupNode(0, m_startX, m_startY);
        if (m_startNode == -1)
        {
            m_destNode = m_startNode;
            return 1;
        }

        m_destNode = LookupNode(0, m_destX, m_destY);
        if (m_startNode != m_destNode)
        {
            int dx = m_startX - m_destX;
            int dy = m_startY - m_destY;

            m_bestNode = m_startNode;
            m_bestDistSq = dy * dy + dx * dx;
            m_budget = budget;
            return 2;
        }
    }

    return 1;
}
