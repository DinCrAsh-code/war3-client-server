//============================================================================
//  0x6F4A2C50 - expand one node of the fine per-cell A*.
//
//  Ask the footprint's own neighbour walk for all eight directions at once,
//  then relax each one that came back.  The two step costs are the octile
//  pair - 15 for a cardinal, 21 for a diagonal, 15 * sqrt(2) rounded - which
//  is the same scale CPathFineSearch::Relax measures its heuristic on.
//
//  The neighbour walk can ask the whole search to stop (m_abort), and that is
//  checked before any of the eight are relaxed rather than after: a walk that
//  gave up part-way through has left the rest of the array unwritten.
//
//  Own translation unit: the four walks and the relax are reached with real
//  calls, and the four walks through a jump table.
//============================================================================
#include "pathfinesearch.h"

//  Cardinal and diagonal step costs.
const int kFineStepCardinal = 15;
const int kFineStepDiagonal = 21;

void CPathFineSearch::Expand(int node)
{
    SCellPoint at(Nodes()[node].m_x, Nodes()[node].m_y);

    int around[8];

    switch (m_footprint)
    {
    case 0:
        Neighbours1(&at, around);
        break;
    case 1:
        Neighbours2(&at, around);
        break;
    case 2:
        Neighbours3(&at, around);
        break;
    case 3:
        Neighbours4(&at, around);
        break;
    }

    if (m_abort != 0)
        return;

    if (around[0] != -1)
        Relax(around[0], node, kFineStepDiagonal);
    if (around[1] != -1)
        Relax(around[1], node, kFineStepCardinal);
    if (around[2] != -1)
        Relax(around[2], node, kFineStepDiagonal);
    if (around[3] != -1)
        Relax(around[3], node, kFineStepCardinal);
    if (around[4] != -1)
        Relax(around[4], node, kFineStepCardinal);
    if (around[5] != -1)
        Relax(around[5], node, kFineStepDiagonal);
    if (around[6] != -1)
        Relax(around[6], node, kFineStepCardinal);
    if (around[7] != -1)
        Relax(around[7], node, kFineStepDiagonal);
}
