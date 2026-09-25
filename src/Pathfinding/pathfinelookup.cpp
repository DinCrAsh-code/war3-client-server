//============================================================================
//  0x6F4A1800 - resolve a cell to a node index for the fine per-cell A*,
//  registering a node for it when this search has not been there yet.
//
//  The bookkeeping rides on the spatial grid's own occupant records rather
//  than a table of its own: a cell's head record, when it is one of the
//  0x02000000 kind, has its second dword reinterpreted as a
//  {search tag, node index} pair.  A record whose tag is this search's is a
//  hit; a record with the wrong tag is re-stamped in place; a cell with no
//  record of that kind at all gets one made for it by the grid.
//
//  The 0x8000 ceiling on the node count is why the node index fits in the
//  record's spare word at all, and is a hard stop: past it the search simply
//  refuses to look any further rather than growing.
//============================================================================
#include "pathfinesearch.h"
#include "spatialgrid.h"

//  The record's second dword under this search's reading of it.  Named
//  separately rather than as a cast at each use so that the two word writes
//  below read as what they are.
struct SAStarCellStamp
{
    int            m_link;      // +0x00  SCellRecord::m_link
    unsigned short m_tag;       // +0x04  which search last claimed this cell
    unsigned short m_node;      // +0x06  the node it made for it
};

//  0x6FA85924 - the node template every registration appends through; only
//  the two coordinates are written, the rest is left as the grid's own
//  zero-initialised copy.
static SAStarNode g_astarFineNode;

int CPathFineSearch::LookupNode(int x, int y)
{
    CCellGrid* grid = m_grid;

    //  One `||` rather than two `if`s: the shipped code has both bounds
    //  failures and the null cell reaching the same single epilogue, and
    //  splitting them makes MSVC duplicate it for the first test - which it
    //  can, because that test is reached before ebp has been pushed.
    int stride = grid->m_stride;
    if ((unsigned int)x >= (unsigned int)stride ||
        (unsigned int)y >= (unsigned int)grid->m_rows)
        return -1;

    int* cell = grid->m_cells + (stride * y + x);
    if (cell == 0)
        return -1;

    SAStarCellStamp* stamp = 0;

    //  `stamp` takes the record first and is cleared again when the kind is
    //  wrong, rather than only being set on the way out of a nested test:
    //  the shipped code re-zeroes the same register the record was in, which
    //  is what this shape gives and the nested one does not.
    unsigned int link = *cell & kCellIndexMask;
    if (link != kCellIndexNone)
    {
        stamp = (SAStarCellStamp*)((SCellRecord*)grid->m_records.m_data + link);

        if ((stamp->m_link & kCellFlagMask) == 0x02000000)
        {
            if (stamp->m_tag == m_searchTag)
                return stamp->m_node;
        }
        else
            stamp = 0;
    }

    unsigned int next = m_nodes.m_count;
    if (next >= 0x8000)
        return -1;

    if (stamp == 0)
        grid->RegisterAStarCell(x, y, m_searchTag, next);
    else
    {
        stamp->m_tag = m_searchTag;
        stamp->m_node = (unsigned short)next;
    }

    g_astarFineNode.m_x = x;
    g_astarFineNode.m_y = y;
    m_nodes.Grow(&g_astarFineNode, 1);

    return next;
}
