//============================================================================
//  0x6F608C50 - CSimpleFrame::ResetLayerPriority: drop one layer's priority
//  back to zero and mark that layer dirty.  Four instructions: the store,
//  the argument written back into its own slot, and the tail jump.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleFrame::ResetLayerPriority(int layer)
{
    m_layerPriority[layer] = 0;
    MarkLayerDirty(layer);
}
