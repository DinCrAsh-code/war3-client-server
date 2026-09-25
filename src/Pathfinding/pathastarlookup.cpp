//============================================================================
//  0x6F498A60 / 0x6F498A00 - resolving a world cell to an A* node.
//
//  The search runs over a four-level grid pyramid, coarsest last.  A cell is
//  looked up at the level the caller asks for; if that cell is *open* - its
//  passability class comes out zero - the walk keeps climbing to coarser
//  levels for as long as they stay open, so that a node covering a large
//  clear area is one node rather than a thousand.  The first level whose cell
//  is not open stops the climb and the last open one is the node.
//
//  The passability class is the top two bits of the cell's flag dword after
//  a per-search shift (m_maskShift is what stages the unit's own PathingType
//  mask into a fixed bit position):
//
//      0x40000000  blocked outright                 -> -1
//      0x80000000  blocked, but only at this level  -> -1 at level 0, else -2
//      0x00000000  open                             -> climb
//
//  Two return values, then, distinguish "there is nothing here" from "there
//  is something, one level up" - which is what lets the caller retry rather
//  than give up.
//============================================================================
#include "pathastar.h"

//----------------------------------------------------------------------------
//  0x6F498A00 - register a fresh node for `cell` and stamp the cell with the
//  current search so that the next lookup finds it instead of registering a
//  second one.
//
//  The node is built in a file-scope scratch object and appended by value:
//  the shipped build has a single static SAStarNode at 0x6FA84FD0 that every
//  registration writes through, not a stack temp.
//----------------------------------------------------------------------------
static SAStarNode g_astarNewNode;    // 0x6FA84FD0

void CPathAStar::RegisterNode(const void* cell, unsigned int level, int x, int y)
{
    SAStarCell* c = (SAStarCell*)cell;

    c->m_stamp = m_searchStamp;
    c->m_node = (unsigned short)m_nodes.m_count;

    g_astarNewNode.m_x = x;
    g_astarNewNode.m_y = y;
    g_astarNewNode.m_type = (unsigned char)level;

    unsigned char linkSlot;
    if (m_useCellLevel != 0)
        linkSlot = c->LevelByte();
    else
        linkSlot = 0;

    g_astarNewNode.m_linkSlot = linkSlot;

    m_nodes.Grow(&g_astarNewNode, 1);
}

//----------------------------------------------------------------------------
//  0x6F498A60 - the lookup itself.
//----------------------------------------------------------------------------
int CPathAStar::LookupNode(unsigned int level, int x, int y)
{
    SAStarCell* cell = m_grids[level]->CellAt(x >> level, y >> level);
    if (cell == 0)
        return -1;

    unsigned int shift = m_maskShift;
    unsigned int cls = (cell->m_word << shift) & 0xC0000000;

    if (cls == 0x40000000)
        return -1;
    if (cls == 0x80000000)
        return -1 - (0 < level);

    unsigned int next = level + 1;
    SAStarGrid** slot = &m_grids[next];
    while (next < 4)
    {
        SAStarCell* up = (*slot)->CellAt(x >> next, y >> next);
        if (((up->m_word << shift) & 0xC0000000) != 0)
            break;

        next++;
        level++;
        slot++;
        cell = up;
    }

    if (cell->m_stamp != m_searchStamp)
        RegisterNode(cell, level, x, y);

    return cell->m_node;
}
