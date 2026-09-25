//============================================================================
//  0x6F4A1EA0 - CPathSearch::VisitCell: the coarse wavefront's own neighbour
//  test.  Bounds- and mask-check the cell exactly the way TestCell does
//  (pathcell.cpp) - it is the same per-cell registration chain, walked here
//  instead of tested outright.  On rejection (off-grid, no cell, or the
//  mask blocks it), a null placeholder is recorded in the frontier (capped
//  at 0x20 entries) so a caller that fans out to several neighbours in one
//  dispatch keeps one frontier slot per neighbour regardless of which one
//  passed.  On acceptance, the chain is walked the same way TestCell walks
//  it: a registration already stamped with this wave, or excluded the same
//  way TestCell excludes one, is stamped and skipped; only a live,
//  mask-matching, not-yet-visited "new box" registration gets its own
//  cached node record (CGridRegistration::m_nodeRecord) enqueued.
//
//  Written with the rejection case as the fall-through and every acceptance
//  step as an explicit forward jump - the shipped code places the (short)
//  rejection handler ahead of the (long) chain walk, which this toolchain
//  only reproduces when the source names that as the ordinary path and the
//  walk as the one being jumped to.
//============================================================================
#include "pathfind.h"

void CPathSearch::VisitCell(int x, int y, SPathFrontier* frontier)
{
    CCellGrid* grid = m_grid;
    unsigned int link;

    if ((unsigned int)x < (unsigned int)grid->m_stride)
    {
        if ((unsigned int)y < (unsigned int)grid->m_rows)
        {
            int* cell = grid->m_cells + (grid->m_stride * y + x);
            if (cell != 0)
            {
                unsigned int word = (unsigned int)*cell;
                if ((word & (unsigned int)m_typeMask & kCellFlagMask) == 0)
                {
                    link = word & kCellIndexMask;
                    goto accepted;
                }
            }
        }
    }

    if (frontier->m_count < 0x20)
    {
        void* none = 0;
        frontier->Grow(&none, 1);
    }
    return;

accepted:
    {
        if (link == kCellIndexNone)
            return;

        grid->m_visitStamp += 1;
        int wave = grid->m_visitStamp;

        do
        {
            SCellRecord* rec = (SCellRecord*)m_grid->m_records.m_data + link;
            unsigned int slot = (unsigned int)rec->m_link;
            unsigned int layer = slot & kCellFlagMask;
            link = slot & kCellIndexMask;

            if (layer == 0x02000000)
                continue;

            CGridRegistration* reg = (CGridRegistration*)rec->m_owner;
            int seen = reg->m_visitStamp;
            if (seen == -1)
                continue;
            if ((reg->m_bits.m_flags & 1) == 0)
                continue;
            if (seen == wave)
                continue;

            reg->m_visitStamp = wave;
            if (layer != 0x01000000)
                continue;

            if ((reg->m_moveFlags & 0x8FFFFFFFu) > 0)
                continue;
            if (((unsigned int)reg->m_typeMask & (unsigned int)m_typeMask &
                 kCellIndexMask) == 0)
                continue;

            EnqueueNode(frontier, reg->m_nodeRecord);
        } while (link != kCellIndexNone);
    }
}
