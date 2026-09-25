//============================================================================
//  0x6F4A0D60 - CPathSearch::TestCell: is one pathing cell free for this
//  search?
//
//  Two rejections and a walk.  The cell is rejected outright when it is off
//  the grid, or when the flag byte the grid keeps in the top of the cell
//  word overlaps the mask the search is running with.  Otherwise the cell's
//  registration chain is walked, and any live registration that is on the
//  "solid" layer, is not excluded by its own movement flags and whose type
//  mask overlaps the search's blocks the cell.
//
//  Two details of the walk are worth naming because they are not obvious
//  from the instructions:
//
//    * the grid's visit stamp is bumped once per call and written into every
//      registration the walk reaches.  A registration already carrying this
//      call's stamp is skipped, so an object registered in more than one of
//      the cells a footprint covers is only tested once per footprint - and
//      a registration carrying -1 is never tested at all;
//    * overlapping the one registration the search is allowed to overlap is
//      not a rejection: it is recorded in m_hitIgnore and the walk carries
//      on with the ordinary tests.  It only reaches that point when the
//      registration is on the solid layer, which is why the note sits
//      inside the layer test and not above it.
//
//  The two `m_blocked = 1; return 0;` exits are written out separately on
//  purpose.  Merging them into one shared label - the usual advice in
//  docs/msvc-vc8-idioms.md - is wrong here: the shipped code has two
//  epilogues with different pop counts, because the chain walk is the only
//  part of the function that needs ebx and ebp and MSVC sinks both saves
//  into it.  One shared exit keeps them live across the whole function and
//  hoists the pushes back into the prologue.
//
//  Its own translation unit: the three footprint walkers call it for real.
//============================================================================
#include "pathfind.h"

int CPathSearch::TestCell(unsigned int x, unsigned int y)
{
    CCellGrid* grid = m_grid;
    //  The stride is a named local because the shipped code loads it once
    //  and uses it both for the bound check and for the row multiply.
    unsigned int stride = (unsigned int)grid->m_stride;

    if (x < stride && y < (unsigned int)grid->m_rows)
    {
        //  The null test survives because nothing here can prove the cell
        //  array is non-null; it is the array's own check, moved onto the
        //  element address by the address arithmetic.
        int* cell = grid->m_cells + (stride * y + x);
        if (cell != 0)
        {
            unsigned int word = (unsigned int)*cell;
            if ((word & (unsigned int)m_typeMask & (unsigned int)kCellFlagMask)
                == 0)
            {
                unsigned int link = word & (unsigned int)kCellIndexMask;
                if (link == (unsigned int)kCellIndexNone)
                    return 1;

                grid->m_visitStamp += 1;
                int stamp = grid->m_visitStamp;

                do
                {
                    //  m_grid re-read every iteration: the shipped code
                    //  does, and nothing in the loop writes it.  This
                    //  toolchain hoists the load out - see
                    //  docs/msvc-vc8-idioms.md, "Where this toolchain
                    //  optimises harder than the shipping one".
                    SCellRecord* rec =
                        (SCellRecord*)m_grid->m_records.m_data + link;
                    unsigned int slot = (unsigned int)rec->m_link;
                    unsigned int layer = slot & (unsigned int)kCellFlagMask;
                    link = slot & (unsigned int)kCellIndexMask;

                    if (layer == 0x02000000)
                        continue;

                    CGridRegistration* reg =
                        (CGridRegistration*)rec->m_owner;
                    int seen = reg->m_visitStamp;
                    if (seen == -1)
                        continue;
                    if ((reg->m_bits.m_flags & 1) == 0)
                        continue;
                    if (seen == stamp)
                        continue;

                    //  Stamped before the layer test, exactly as the
                    //  shipped code schedules it: a registration seen on
                    //  any layer counts as seen for the rest of this call.
                    reg->m_visitStamp = stamp;
                    if (layer != 0x01000000)
                        continue;

                    if (reg == m_ignore)
                        m_hitIgnore = 1;

                    unsigned int flags = reg->m_moveFlags;
                    if ((flags & 0x8FFFFFFFu) > 0)
                        continue;
                    if (m_searching == 0 && (flags & 0x60000000u) != 0)
                        continue;
                    if (((unsigned int)reg->m_typeMask &
                         (unsigned int)m_typeMask &
                         (unsigned int)kCellIndexMask) != 0)
                    {
                        m_blocked = 1;
                        return 0;
                    }
                } while (link != (unsigned int)kCellIndexNone);

                return 1;
            }
        }
    }
    m_blocked = 1;
    return 0;
}
