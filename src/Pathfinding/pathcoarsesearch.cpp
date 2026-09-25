//============================================================================
//  0x6F49D8D0 - CPathClient::CoarseSearch: one pass of the masked wavefront
//  flood.
//
//  Builds the direction+distance from `from` to `to`, converts `from` to
//  cell coordinates, clears any earlier frontier, stages this client's own
//  mask onto the shared CPathSearch singleton, dispatches to the quality
//  tier's neighbour scan (word_6FAB774C picks one of the four
//  VisitNeighbours* families) around the cell the client is standing on,
//  then lets CPathClient::TrackBest pick the best candidate the scan
//  produced. Returns whether the frontier ended up empty (no candidate at
//  all - the caller treats that as "nothing reachable this pass").
//
//  `this->m_ignore`'s refcount is bumped for the duration and released
//  again on every exit, which is a `__try`/`__finally` in the shipped
//  build; this toolchain cannot reproduce that frame's exact shape at all
//  under this repo's fixed /GS- /EHs-c- (docs/msvc-vc8-idioms.md, "An
//  `__except_handler4`-shaped frame this toolchain cannot reproduce"), so
//  the release is written as a plain unconditional cleanup instead.
//============================================================================
#include "pathmove.h"

int CPathClient::CoarseSearch(const CGridVec2* from, const CGridVec2* to)
{
    CFloat dy = to->m_y - from->m_y;
    CFloat dx = to->m_x - from->m_x;

    CGridVec2 delta(dx, dy);
    NormalizeVec2(&delta);

    //  y before x, and through named temporaries: the shipped code floors and
    //  converts the *y* component first at both cells, then stores the pair.
    int fromY = CFloatToInt(CFloatFloor(from->m_y));
    int fromX = CFloatToInt(CFloatFloor(from->m_x));
    SCellPoint fromCell(fromX, fromY);

    //  The shipped code builds a *second* cell point one normalised step away
    //  from `from` - two CFloat::operator+ calls and a second Floor/ToInt pair
    //  - and it is that pair of cells, not one cell against itself, that
    //  DirectionBetween is handed.  Leaving it out was worth ten instructions
    //  and made the direction argument meaningless.
    CGridVec2 stepped(from->m_x + delta.m_x, from->m_y + delta.m_y);

    int toY = CFloatToInt(CFloatFloor(stepped.m_y));
    int toX = CFloatToInt(CFloatFloor(stepped.m_x));
    SCellPoint toCell(toX, toY);

    CPathSearch* search = g_pPathSearch;
    SPathFrontier* frontier = &search->m_frontier;
    if (frontier->m_count != 0)
        frontier->EraseRange(0, frontier->m_count);

    int dir = DirectionBetween(&toCell.m_x, &fromCell.m_x);

    CGridRegistration* ignore = m_ignore;
    if (ignore != 0)
        ignore->m_cellCount += 1;

    search->m_typeMask = m_typeMask;

    switch (g_pathQuality)
    {
    case 0: search->VisitNeighbours1(&fromCell, dir, frontier); break;
    case 1: search->VisitNeighbours2(&fromCell, dir, frontier); break;
    case 2: search->VisitNeighbours3(&fromCell, dir, frontier); break;
    case 3: search->VisitNeighbours4(&fromCell, dir, frontier); break;
    }

    TrackBest(frontier);

    int empty = (frontier->m_count == 0);

    if (ignore != 0)
        ignore->m_cellCount -= 1;

    return empty;
}
