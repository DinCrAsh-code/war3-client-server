//============================================================================
//  0x6F606160 - CLayoutFrame::ClearPoints.  Its own translation unit because
//  SetAllPoints calls it.
//============================================================================
#include "frame.h"
#include "framethunks.h"

//  0x6F606160 - release every frame point this frame holds, telling each
//  point's anchor frame to forget this one first.
void CLayoutFrame::ClearPoints()
{
    unsigned int remaining = m_pointCount;
    CFramePoint** slot = m_points;

    while (remaining)
    {
        CFramePoint* point = *slot;
        --remaining;
        if (point)
        {
            CLayoutFrame* anchor = point->GetRelativeFrame();
            if (anchor)
                anchor->RemoveDependency(this);
            //  Re-read: the shipped code loads the slot again rather than
            //  reusing the value it tested, which is what the anchor call in
            //  between makes necessary.
            (*slot)->Release();
            *slot = 0;
        }
        ++slot;
    }
}

