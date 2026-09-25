//============================================================================
//  0x6F60C2A0 - CSimpleTop::AddFrame: remember one frame in the all-frames
//  list at +0x174, once.
//============================================================================
#include "frame.h"
#include "framethunks.h"

static const char kSimpleFrameNodeTypeName[] = ".?AUSIMPLEFRAMENODE@@";

void CSimpleTop::AddFrame(CSimpleFrame* frame)
{
    int link = (int)m_frames.Tail();

    while (link > 0)
    {
        SIMPLEFRAMENODE* existing = (SIMPLEFRAMENODE*)link;
        void* owner = existing->m_pFrame;

        if (owner == frame)
            return;

        link = (int)m_frames.Prev(existing);
    }

    SIMPLEFRAMENODE* node = (SIMPLEFRAMENODE*)
        g_simpleFrameNodeAllocator.Alloc(1, kSimpleFrameNodeTypeName, -2);

    TSLink<SIMPLEFRAMENODE>* nodeLink;
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
        nodeLink = (TSLink<SIMPLEFRAMENODE>*)&m_frames.m_terminator;
    }

    nodeLink->Unlink();

    TSLink<SIMPLEFRAMENODE>* head = m_frames.m_terminator.m_next;
    nodeLink->m_next = head;
    nodeLink->m_prevlink = head->m_prevlink;
    head->m_prevlink = (int)node;
    m_frames.m_terminator.m_next = nodeLink;

    node->m_pFrame = frame;
}
