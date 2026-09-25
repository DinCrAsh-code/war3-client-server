//============================================================================
//  0x6F6054A0 - CLayoutFrame::SetRect, slot 3 (+0x0C).  Mark m_54 (the
//  "have I ever had a rect" flag slot 4/GetRect and slot 0's own cascade
//  both test), dispatch slot 0 *with the old m_rect still live* so it can
//  diff against the incoming one, then store the new rect, then - only for
//  a frame with a backdrop generator (m_64 != 0, set by SetOwnedFrame) -
//  rebuild it.  Always returns m_54 (1); see the declaration's own comment
//  for why the reconstruction keeps that instead of declaring `void`.
//============================================================================
#include "frame.h"
#include "framethunks.h"

int CSimpleFrame::SetRect(const SFrameRect* rect)
{
    m_54 = 1;
    DestroyLayout((int)rect);

    int hasBackdrop = (m_64 != 0);

    //  Raw 32-bit copies, not `fld`/`fstp` pairs - see GetRect's own
    //  comment (simpleframevtable4.cpp) for why.
    ((int*)m_rect)[0] = ((const int*)rect)[0];
    ((int*)m_rect)[1] = ((const int*)rect)[1];
    ((int*)m_rect)[2] = ((const int*)rect)[2];
    ((int*)m_rect)[3] = ((const int*)rect)[3];

    if (hasBackdrop)
        RebuildBackdropRect(1);

    return m_54;
}
