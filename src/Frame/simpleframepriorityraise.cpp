//============================================================================
//  0x6F608C30 - CSimpleFrame::RaiseLayerPriority: ResetLayerPriority's own
//  "raise to 1" counterpart (simpleframepriority.cpp) - same shape, the
//  opposite constant.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleFrame::RaiseLayerPriority(int layer)
{
    m_layerPriority[layer] = 1;
    MarkLayerDirty(layer);
}
