//============================================================================
//  0x6F60E8A0 - CSimpleRegion::MarkLayerDirty: seven instructions, a tail
//  call into the parent frame's own version.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleRegion::MarkLayerDirty()
{
    if ((m_regionFlags & 1) != 0)
        m_pParent->MarkLayerDirty(m_layer);
}
