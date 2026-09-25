//============================================================================
//  0x6F60E7C0 / 0x6F60E7E0 - CSimpleRegion::AttachToLayer and
//  ::DetachFromLayer: join or leave the parent frame's list for my layer,
//  once each way.  Bit 0 of m_regionFlags is the "am I in it" flag.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleRegion::AttachToLayer()
{
    if ((m_regionFlags & 1) == 0)
    {
        m_pParent->AddLayerRegionNode(this, m_layer);
        m_regionFlags |= 1;
    }
}

void CSimpleRegion::DetachFromLayer()
{
    if ((m_regionFlags & 1) != 0)
    {
        m_pParent->RemoveLayerRegionNode(this, m_layer);
        m_regionFlags &= ~1u;
    }
}
