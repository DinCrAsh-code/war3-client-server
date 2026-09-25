//============================================================================
//  0x6F6061B0 / 0x6F606770 / 0x6F6067F0 - the three point setters.  None of
//  them calls another, so they share a translation unit; everything they do
//  call (RemoveDependency, ClearPoints, AddDependency, QueueForLayout) is in
//  one of its own next door.
//============================================================================
#include "frame.h"
#include "framethunks.h"

//  0x6F6061B0 - anchor point `index` at an absolute position.
void CLayoutFrame::SetPoint(int index, float x, float y, int relayout)
{
    CFramePoint* old = m_points[index];
    if (old)
    {
        CLayoutFrame* anchor = old->GetRelativeFrame();
        if (anchor)
            anchor->RemoveDependency(this);
        old->Release();
    }

    m_points[index] = MakeAbsoluteFramePoint(x, y);

    if (relayout)
        QueueForLayout(0);
}

//  0x6F606770 - anchor point `index` to point `otherPoint` of `other`.
void CLayoutFrame::SetPointTo(int index, CLayoutFrame* other, int otherPoint,
                              float x, float y, int relayout)
{
    CFramePoint* old = m_points[index];
    if (old)
    {
        CLayoutFrame* anchor = old->GetRelativeFrame();
        if (anchor)
            anchor->RemoveDependency(this);
        old->Release();
    }

    m_points[index] = MakeRelativeFramePoint(other, otherPoint, x, y);
    other->AddDependency(this, 0x0F);

    if (relayout)
        QueueForLayout(0);
}

//  0x6F6067F0 - the "cover this other frame exactly" shorthand: drop every
//  point and re-anchor points 0 and 8 - the two opposite corners - to the
//  same two of `other`, at no offset.
void CLayoutFrame::SetAllPoints(CLayoutFrame* other, int relayout)
{
    ClearPoints();

    m_points[0] = MakeRelativeFramePoint(other, 0, 0.0f, 0.0f);
    m_points[8] = MakeRelativeFramePoint(other, 8, 0.0f, 0.0f);
    other->AddDependency(this, 0x0F);

    if (relayout)
        QueueForLayout(0);
}

