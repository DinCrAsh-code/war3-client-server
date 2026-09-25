//============================================================================
//  0x6F608970 - CLayoutFrame::PointInRect, slot 10 (+0x28).  Once this
//  frame's cached screen rect is valid (m_54 != 0, set by SetRect/slot 3),
//  is `point` inside it?
//============================================================================
#include "frame.h"
#include "framethunks.h"

int CSimpleFrame::PointInRect(const float* p) const
{
    if (m_54 != 0)
    {
        if (RectContainsPoint(&m_screenX0, 0, p))
            return 1;
        return 0;
    }
    return 0;
}
