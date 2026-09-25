//============================================================================
//  0x6F49C2E0 - CPathClient::PopWaypoint: consume one queued waypoint off
//  the tail of the second cache, optionally shifting the cursor back by the
//  queued run's own length first.
//
//  With nothing queued, this is a no-op success. Otherwise, when the
//  caller allows a step (`allowStep`) and the tail actually has a queued
//  run (TailWaypoint() nonzero) whose type bit is set, the point two
//  behind the cursor is scaled from grid space to world space and handed
//  to PlaceWaypoint; *movedOut reports whether that placement moved
//  anything, and a placement failure aborts the whole call before the
//  cursor is touched. Every other path (the type bit clear, or
//  allowStep == 0) falls through to unconditionally retire two cache
//  slots and re-run TrimTail so the tail stays clean.
//============================================================================
#include "pathmove.h"

CGridVec2 __fastcall ScaleGridVec2(const CGridVec2& v, const CFloat& scale)
{
    return CGridVec2(v.m_x * scale, v.m_y * scale);
}

int CPathClient::PopWaypoint(int allowStep, int* movedOut)
{
    *movedOut = 0;
    if (m_cursor[1] == 0)
        return 1;

    if (allowStep)
    {
        unsigned char step = TailWaypoint();
        if (step > 0)
        {
            if (PathOwnerSingleton()->TestTypeRecordBit(step))
            {
                CFloat cellSize = PathOwnerSingleton()->m_grid->m_cellSize;
                CGridVec2 worldDelta = ScaleGridVec2(m_cache[1].Points()[m_cursor[1] - 2], cellSize);

                int moved = PlaceWaypoint(&worldDelta);
                *movedOut = moved;
                if (moved == 0)
                    return 0;
            }

            m_cursor[1] -= 2;
        }
    }

    int outFlag;
    m_cursor[1] = TrimTail(&outFlag, 0);
    return 1;
}
