//============================================================================
//  0x6F4A3AD0 - set the fine search up and classify the query.
//
//  CPathAStar::Setup (pathastarsetup.cpp) on the other context, with three
//  differences worth knowing:
//
//    * the footprint size is derived here rather than handed in - the
//      caller passes a radius and ClassifySlopeAxis turns it into the 0-3
//      index CPathFineSearch::Expand switches on;
//    * the search tag is a *word*, incremented in place, where the other
//      context's stamp is a dword;
//    * neither node lookup is checked.  The other Setup gives up when the
//      start has no node and answers "trivial" when both endpoints resolve
//      to the same one; this one takes whatever comes back and always
//      answers 2 once the two cells differ.
//============================================================================
#include "pathfinesearch.h"
#include "boxmath.h"

int CPathFineSearch::Setup(const CGridVec2* from, const CGridVec2* to,
                           int budget, const int* mask, const CFloat* radius,
                           int typeFlags)
{
    Reset(0);

    CGridVec2* start = &m_from;
    *start = *from;

    CGridVec2* goal = &m_to;
    *goal = *to;

    int startY = CFloatToInt(CFloatFloor(start->m_y));
    m_startX = CFloatToInt(CFloatFloor(start->m_x));
    m_startY = startY;

    int destY = CFloatToInt(CFloatFloor(goal->m_y));
    m_destX = CFloatToInt(CFloatFloor(goal->m_x));
    m_destY = destY;

    if (m_grid->CellAt(m_startX, m_startY) == 0)
        return 0;

    if (m_startX == m_destX && m_startY == m_destY)
        return 1;

    m_footprint = (unsigned short)ClassifySlopeAxis(radius);

    m_searchTag++;
    m_typeMask = *mask;
    m_typeFlags = typeFlags;

    m_abort = 0;
    m_reservedD0 = 0;
    m_reservedD4 = 0;

    m_startNode = LookupNode(m_startX, m_startY);
    m_destNode = LookupNode(m_destX, m_destY);

    //  dx declared first: the shipped code computes the y difference first
    //  all the same, and writing them the other way round gets the two
    //  subtractions - and the register each lands in - the wrong way about.
    //  Same measurement as CPathAStar::Setup's own pair.
    int dx = m_startX - m_destX;
    int dy = m_startY - m_destY;

    m_bestDistSq = dy * dy + dx * dx;
    m_budget = budget;
    m_bestNode = m_startNode;

    return 2;
}
