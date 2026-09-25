//============================================================================
//  0x6F49F6C0 - claim a cell for the fine search.
//
//  CCellGrid::RegisterCell (spatialgrid.cpp) with one difference, and it is
//  the interesting one: where that stores an owning CGridRegistration* in the
//  record's second dword, this stores a {search tag, node index} pair of
//  words there.  That is the same reinterpretation CPathFineSearch::LookupNode
//  (pathfinelookup.cpp) reads back out, and it is how the search gets a
//  per-cell scratch slot without a table of its own.
//
//  The record kind is always 0x02000000 - there is no `flag` argument to
//  choose it, which is what makes the kind test in LookupNode and in
//  CPathSearch::CellReaches meaningful.  The occupancy bit is set
//  unconditionally for the same reason: RegisterCell only sets it for flag 0.
//============================================================================
#include "spatialgrid.h"

void CCellGrid::RegisterAStarCell(int x, int y, unsigned int tag,
                                  unsigned int node)
{
    int cell = m_stride * y + x;

    m_occupancy[(unsigned int)cell >> 5] |= 1u << (cell & 31);

    int record = m_freeRecord;
    int* head = &m_cells[cell];

    if (record == kCellIndexNone)
    {
        record = (int)m_records.m_count;

        SCellRecord fresh;
        fresh.m_link = (*head & kCellIndexMask) | 0x02000000;
        ((unsigned short*)&fresh)[2] = (unsigned short)tag;
        ((unsigned short*)&fresh)[3] = (unsigned short)node;
        m_records.AppendRecords(&fresh, 1);
    }
    else
    {
        SCellRecord* slot = (SCellRecord*)m_records.m_data + record;
        m_freeRecord = slot->m_link & kCellIndexMask;
        slot->m_link = (*head & kCellIndexMask) | 0x02000000;
        ((unsigned short*)slot)[2] = (unsigned short)tag;
        ((unsigned short*)slot)[3] = (unsigned short)node;
    }

    m_liveRecords += 1;
    *head = (*head & kCellFlagMask) | record;
}
