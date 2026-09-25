//============================================================================
//  0x6F47D8A0 / 0x6F47DAD0 - one cell's registration chain, and the
//  "collect it all again" front door.
//============================================================================
#include "agentquery.h"

//----------------------------------------------------------------------------
//  0x6F47D8A0 - walk the chain hanging off one cell and append every
//  registration this query wants.
//
//  Two stamps are in play, which is what the second `[edi+0xB4]` bump is
//  for.  `stamp` is the walk's own, handed down from CollectBox: a
//  registration marked with it has been *accepted* by this walk and must not
//  be looked at again in another of the walk's cells.  The one this function
//  makes for itself marks a registration as *rejected*, and it is fresh per
//  cell so that the rejection is remembered only for as long as the chain
//  being walked.  A registration already carrying -1 is never a candidate at
//  all.
//
//  A 0x02000000 chain entry is a search's own head record rather than a
//  registration (see spatialgrid.h) and is stepped straight over; a
//  0x01000000 one is the layer that gets tested, and anything else is
//  rejected outright.
//----------------------------------------------------------------------------
void AgileAgentQuery::CollectCell(CCellGrid* grid, int cell, int stamp)
{
    int index = grid->m_cells[cell] & kCellIndexMask;
    if (index == kCellIndexNone)
        return;

    grid->m_visitStamp++;
    int rejected = grid->m_visitStamp;

    do
    {
        SCellRecord* record = (SCellRecord*)grid->m_records.m_data + index;
        int link = record->m_link;
        index = link & kCellIndexMask;

        int layer = link & kCellFlagMask;
        if (layer != 0x02000000)
        {
            CGridRegistration* reg = (CGridRegistration*)record->m_owner;
            if (reg->m_visitStamp != stamp &&
                reg->m_visitStamp != rejected &&
                reg->m_visitStamp != -1)
            {
                if (layer == 0x01000000)
                {
                    unsigned int bits = reg->ClassBits() & m_excludeMask;
                    bits |= (reg->m_moveFlags >> 28) & 1;
                    if (bits == 0)
                    {
                        SAgentQueryHit hit;
                        hit.m_reg = reg;
                        hit.m_classBits = bits;
                        AppendEntries(&hit, 1);
                    }
                    reg->m_visitStamp = stamp;
                }
                else
                {
                    reg->m_visitStamp = rejected;
                }
            }
        }
    }
    while (index != kCellIndexNone);
}

//----------------------------------------------------------------------------
//  0x6F47DAD0 - throw the collected set away and collect it again, for
//  whichever rectangle and grid the query is currently pointed at.
//
//  The anchored form is not an alternative *box*: an anchored query follows
//  its registration's grid as well, which is why both the grid and the
//  rectangle are chosen by the same test.
//----------------------------------------------------------------------------
void AgileAgentQuery::Refresh()
{
    Clear();

    //  Two separate conditionals on the same anchor, not one branch that
    //  picks both: the shipped code tests it twice, which is what keeping
    //  them as two independent expressions produces.
    CCellGrid* grid = m_anchor != 0 ? m_anchor->m_grid : m_grid;
    const SCellBox* box = m_anchor != 0 ? &m_anchor->m_box : &m_box;

    if (grid != 0 && box->m_y0 <= box->m_y1 && box->m_x0 <= box->m_x1)
        CollectBox(grid, box);
}
