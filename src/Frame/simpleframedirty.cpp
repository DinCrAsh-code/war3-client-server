//============================================================================
//  0x6F6088C0 - CSimpleFrame::MarkLayerDirty: set my own (depth, layer) cell
//  of CSimpleTop's dirty grid.
//
//  Its own translation unit: five members of this batch call it for real.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleFrame::MarkLayerDirty(int layer)
{
    CSimpleTop* top = m_pTop;
    if (top == 0)
        return;

    if (m_attached == 0)
        return;

    int depth = m_depth;
    top->m_dirty[layer + depth * 5] = 1;
}
