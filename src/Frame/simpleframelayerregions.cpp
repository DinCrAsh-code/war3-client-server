//============================================================================
//  0x6F609580 / 0x6F609850 - CSimpleFrame::AddLayerRegionNode and
//  ::RemoveLayerRegionNode: the five per-layer region lists at +0xDC.
//
//  Adding one first hands the region this frame's current scale through the
//  region's own vtable slot 5, then links it and marks the layer dirty.
//============================================================================
#include "frame.h"
#include "framethunks.h"

static const char kRegionNodeTypeName[] = ".?AUREGIONNODE@@";

void CSimpleFrame::AddLayerRegionNode(void* region, int layer)
{
    ((LayoutSetScaleFn)(*(void***)region)[0x14 / 4])(region, m_scale);

    REGIONNODE* node =
        (REGIONNODE*)g_regionNodeAllocator.Alloc(1, kRegionNodeTypeName, -2);
    SFrameSlot* list = &m_layerRegions[layer];

    TSLink<REGIONNODE>* nodeLink;
    if (node != 0)
    {
        //  The stores go through `node` and the link is formed *after*
        //  them: a node whose link is its first member needs no `lea`, and
        //  naming the link first moves the `mov ecx, eax` above the pair.
        node->m_link.m_next = 0;
        node->m_link.m_prevlink = 0;
        nodeLink = &node->m_link;
    }
    else
    {
        nodeLink = (TSLink<REGIONNODE>*)&list->m_terminator;
    }

    nodeLink->Unlink();

    TSLink<REGIONNODE>* head = list->m_terminator.m_next;
    nodeLink->m_next = head;
    nodeLink->m_prevlink = head->m_prevlink;
    head->m_prevlink = (int)node;
    list->m_terminator.m_next = nodeLink;

    node->m_pRegion = region;

    MarkLayerDirty(layer);
}

void CSimpleFrame::RemoveLayerRegionNode(void* region, int layer)
{
    SFrameSlot* list = &m_layerRegions[layer];

    int link = (int)list->Tail();

    while (link > 0)
    {
        REGIONNODE* node = (REGIONNODE*)link;

        if (node->m_pRegion == region)
        {
            MarkLayerDirty(layer);
            list->RemoveNode(node);
            return;
        }

        link = (int)list->Prev(node);
    }
}
