//============================================================================
//  0x6F604F70 - CLayoutFrame::GetRect, slot 4 (+0x10).  1 and a copy of
//  m_rect once SetRect (slot 3) has run at least once (m_54 != 0); 0
//  otherwise.  No callees.
//============================================================================
#include "frame.h"
#include "framethunks.h"

int CSimpleFrame::GetRect(SFrameRect* out) const
{
    if (m_54 == 0)
        return 0;

    //  A raw 32-bit copy per field, not an `fld`/`fstp` pair: the shipped
    //  body treats the four floats as opaque dwords here (`mov`, not `fld`),
    //  which is what a plain `out->x0 = m_rect[0]` does not reproduce.
    ((int*)out)[0] = ((int*)m_rect)[0];
    ((int*)out)[1] = ((int*)m_rect)[1];
    ((int*)out)[2] = ((int*)m_rect)[2];
    ((int*)out)[3] = ((int*)m_rect)[3];
    return 1;
}
