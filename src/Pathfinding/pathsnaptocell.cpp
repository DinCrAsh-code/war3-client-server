//============================================================================
//  0x6F49C460 - CPathClient::SnapToCell: is the destination close enough to
//  a region node to count as "arrived"?
//
//  `node` defaults to this client's own target node when the caller passes
//  none, and a node whose registration has been retired (visit stamp -1) is
//  no answer at all.  Otherwise the destination is floored into a cell and
//  the ring around it is scanned - with the ring's radius and side length
//  taken from the global path quality, so a lower-quality search accepts a
//  looser arrival.
//
//  The four arms really are four separate constant pairs rather than a
//  table lookup: (1,3), (2,4), (2,5), (3,6).
//============================================================================
#include "pathmove.h"
#include "pathfind.h"
#include "spatialgrid.h"

int CPathClient::SnapToCell(const CGridVec2* to, CGridRegistration* node)
{
    if (node == 0)
    {
        node = m_targetNode;
        if (node == 0)
            return 0;
    }

    if (node->m_visitStamp == -1)
        return 0;

    SCellPoint cell;
    g_pPathGrid->FloorToCell(&cell, to);

    switch (g_pathQuality)
    {
    case 0:
        return g_pPathSearch->ScanRing(cell.m_x, cell.m_y, node, 1, 3);
    case 1:
        return g_pPathSearch->ScanRing(cell.m_x, cell.m_y, node, 2, 4);
    case 2:
        return g_pPathSearch->ScanRing(cell.m_x, cell.m_y, node, 2, 5);
    case 3:
        return g_pPathSearch->ScanRing(cell.m_x, cell.m_y, node, 3, 6);
    }

    return 0;
}
