//============================================================================
//  0x6F606210 - CLayoutFrame::RemovePointsRelativeTo.  ClearPoints's own
//  selective sibling (Frame/frameclearpoints.cpp): drop only the anchor
//  points whose GetRelativeFrame() answers a given frame, then optionally
//  queue this frame for re-layout.  The field evidence is exact: m_54 (the
//  field this file's own QueueForLayout gate reads) and m_pointCount/
//  m_points (frame.h) are the same fields ClearPoints already uses at the
//  same offsets.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CLayoutFrame::RemovePointsRelativeTo(CLayoutFrame* frame, int requeue)
{
    unsigned int remaining = m_pointCount;
    CFramePoint** slot = m_points;

    if (remaining)
    {
        do
        {
            CFramePoint* point = *slot;
            --remaining;
            if (point)
            {
                CLayoutFrame* anchor = point->GetRelativeFrame();
                if (anchor == frame)
                {
                    (*slot)->Release();
                    *slot = 0;
                }
            }
            ++slot;
        } while (remaining);
    }

    if (requeue)
        QueueForLayout(0);
}
