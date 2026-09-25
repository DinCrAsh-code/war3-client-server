//============================================================================
//  0x6F4A2F30 - turn a finished search into a route.
//
//  This is what the whole fine search is for, and it is the clearest single
//  piece of evidence that it really is an A* and not a best-point flood: it
//  walks the parent chain back from the node the search stopped on, appending
//  each cell's centre as it goes.  Every node on that chain got its m_parent
//  from CPathFineSearch::Relax, and -1 is where the start node's chain ends.
//
//  The two ends are then fixed up, because a route made of cell centres does
//  not start or end where the caller actually wanted:
//
//    * the last point appended - the one for the node the walk *ended* on,
//      which is the start of the route in travel order - is replaced with
//      `from` outright;
//    * the first point appended is replaced with `to` only if it lands in the
//      same cell, so that a unit already inside the goal cell walks to the
//      exact point rather than to the middle of the cell.  Same cell is
//      tested by flooring both coordinates, which is why four CFloatFloor /
//      CFloatToInt pairs sit at the end of an otherwise integer function.
//
//  Four named CFloat temporaries, not four nested calls: the shipped code
//  gives each floor its own stack slot, the same way
//  CCellGrid::FloatBoxToCellBox (boxmath.cpp) does.
//============================================================================
#include "pathfinesearch.h"
#include "boxmath.h"

//  The half a cell each axis of a cell centre is offset by.
extern const CFloat g_CFloatHalf;       // dword_6FAAE4D0

void CPathFineSearch::BuildRoute(int node, SPathPointCache* out,
                                 const CGridVec2* from, const CGridVec2* to)
{
    do
    {
        int y = Nodes()[node].m_y;
        SAStarNode* n = &Nodes()[node];

        CFloat cy = CFloatFromInt(y) + g_CFloatHalf;
        CFloat cx = CFloatFromInt(n->m_x) + g_CFloatHalf;

        CGridVec2 centre;
        centre.m_x = cx;
        centre.m_y = cy;

        out->Grow(&centre, 1);

        node = n->m_parent;
    }
    while (node != -1);

    //  Travel order is the reverse of walk order, so the last slot is where
    //  the unit starts from.
    out->Points()[out->m_count - 1] = *from;

    //  Nested, not four named temporaries: the shipped code feeds
    //  CFloatFloor's own return value straight into CFloatToInt rather than
    //  re-taking the address of a local, which is the one place this differs
    //  from CCellGrid::FloatBoxToCellBox's otherwise identical run of four.
    int wantYCell = CFloatToInt(CFloatFloor(to->m_y));
    int wantXCell = CFloatToInt(CFloatFloor(to->m_x));

    CGridVec2* head = out->Points();

    int gotYCell = CFloatToInt(CFloatFloor(head->m_y));
    int gotXCell = CFloatToInt(CFloatFloor(head->m_x));

    if (gotXCell == wantXCell && gotYCell == wantYCell)
        *out->Points() = *to;
}
