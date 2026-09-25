//============================================================================
//  0x6F60E800 - CSimpleRegion::SetParentLayer: move this region to another
//  frame, another layer, or just in or out of the one it is in.
//
//  Three cases, and the shipped code's own three blocks: a different parent
//  re-registers from scratch, the same parent with a different layer only
//  has to leave the old one, and the same parent and layer reduces to
//  "should I be linked at all".
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleRegion::SetParentLayer(CSimpleFrame* parent, int layer, int attach)
{
    CSimpleFrame* old = m_pParent;

    if (old != parent)
    {
        if (old != 0)
        {
            DetachFromLayer();
            //  Re-read: the shipped code loads m_pParent again rather than
            //  keeping `old` live across the call.
            m_pParent->RemoveRegionNode(this);
        }

        m_pParent = parent;
        m_layer = layer;

        if (parent == 0)
            return;

        parent->AddRegionNode(this);
        NotifyColorChanged();
    }
    else if (m_layer != layer)
    {
        if ((m_regionFlags & 1) != 0)
            DetachFromLayer();

        m_layer = layer;
    }
    else
    {
        int linked = ((m_regionFlags & 1) == 1);

        if (attach == linked)
            return;

        if (attach != 0)
            AttachToLayer();
        else
            DetachFromLayer();

        return;
    }

    if (attach != 0)
        AttachToLayer();
}
