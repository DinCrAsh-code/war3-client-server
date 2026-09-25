//============================================================================
//  0x6F4A0C20 - "is `owner` registered in this cell, and is its registration
//  the directly-reachable kind".
//
//  Walks the cell's own record chain looking for one whose second dword is
//  `owner`, skipping the 0x02000000 records outright - those are the search's
//  own per-cell scratch stamps (pathastarcellregister.cpp), whose second
//  dword is a {tag, node} pair and not a pointer at all, so comparing it
//  against an owner would be comparing two unrelated things.
//
//  The answer is not "found" but "found *and* its kind byte is 0x01000000",
//  which is what the coarse search's fine snap
//  (CPathClient::SnapToCell -> CPathSearch::ScanRing) asks: not whether the
//  target region touches this cell, but whether it touches it *directly*.
//============================================================================
#include "pathfind.h"
#include "spatialgrid.h"

int CPathSearch::CellReaches(const int* cell, const void* owner)
{
    if (owner == 0)
        return 0;

    unsigned int link = *cell & kCellIndexMask;
    if (link == kCellIndexNone)
        return 0;

    SCellRecord* records = (SCellRecord*)m_grid->m_records.m_data;

    for (;;)
    {
        unsigned int word = records[link].m_link;
        SCellRecord* record = &records[link];

        //  `link` is advanced to the next record *before* the owner test,
        //  not after it.  That is what makes the shipped code form the
        //  record's address into a register of its own (`lea ecx,
        //  [esi+edx*8]`) instead of folding it into the compare: by the time
        //  the compare runs, the index it would have been folded from has
        //  already been overwritten with the next one.  Writing the two the
        //  other way round costs two instructions and reads no better.
        link = word & kCellIndexMask;
        unsigned int kind = word & kCellFlagMask;

        if (kind != 0x02000000 && record->m_owner == owner)
            return kind == 0x01000000;

        if (link == kCellIndexNone)
            return 0;
    }
}
