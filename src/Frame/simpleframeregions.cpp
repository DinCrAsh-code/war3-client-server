//============================================================================
//  0x6F6094F0 / 0x6F609810 - CSimpleFrame::AddRegionNode and
//  ::RemoveRegionNode: the unlayered region list at +0xD0.
//
//  The add has the same "fill the node inside Link()'s own null test" shape
//  CLayoutFrame::AddDependent does, one level simpler because a REGIONNODE
//  carries no vtable and its link is therefore its first member.
//============================================================================
#include "frame.h"
#include "framethunks.h"

static const char kRegionNodeTypeName[] = ".?AUREGIONNODE@@";

void CSimpleFrame::AddRegionNode(void* region)
{
    REGIONNODE* node =
        (REGIONNODE*)g_regionNodeAllocator.Alloc(1, kRegionNodeTypeName, -2);

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
        nodeLink = (TSLink<REGIONNODE>*)&m_regions.m_terminator;
    }

    nodeLink->Unlink();

    TSLink<REGIONNODE>* head = m_regions.m_terminator.m_next;
    nodeLink->m_next = head;
    nodeLink->m_prevlink = head->m_prevlink;
    head->m_prevlink = (int)node;
    m_regions.m_terminator.m_next = nodeLink;

    node->m_pRegion = region;
}

void CSimpleFrame::RemoveRegionNode(void* region)
{
    int link = (int)m_regions.Tail();

    while (link > 0)
    {
        REGIONNODE* node = (REGIONNODE*)link;

        if (node->m_pRegion == region)
        {
            m_regions.RemoveNode(node);
            return;
        }

        link = (int)m_regions.Prev(node);
    }
}
