//============================================================================
//  0x6F609980 - CSimpleFrame::SetParent: re-parent this frame, taking the
//  new parent's depth and scale as it goes.
//
//  The vtable pointer is read *before* the branch that picks the scale,
//  which is the shipped code's own scheduling and what keeps `this` in esi
//  across both arms.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleFrame::SetParent(void* parentFrame)
{
    CSimpleFrame* parent = (CSimpleFrame*)parentFrame;
    CSimpleFrame* old = m_pParent;
    int attached = m_attached;

    if (old != 0)
        old->RemoveChildNode(this);

    if (attached != 0)
    {
        m_shown = 0;
        NotifyHidden();
    }

    m_pParent = parent;

    float scale;
    if (parent != 0)
    {
        m_depth = parent->m_depth + 1;
        scale = parent->m_scale;
    }
    else
    {
        scale = 1.0f;
        m_depth = 0;
    }

    ((LayoutSetScaleFn)(*(void***)this)[0x14 / 4])(this, scale);

    if (m_pParent != 0)
        m_pParent->AddChildNode(this);

    if (attached != 0)
    {
        CSimpleFrame* now = m_pParent;
        if (now == 0 || now->m_attached != 0)
        {
            m_shown = 1;
            NotifyConstructed();
        }
    }
}
