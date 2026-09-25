//============================================================================
//  0x6F609630 / 0x6F6098B0 - CSimpleFrame::AddChildNode and
//  ::RemoveChildNode: the child list at +0x118.
//
//  The removal's own unlink-and-free helper (0x6F6096C0) has no `### ... @`
//  heading of its own: IDA models it as a *tail chunk* of RemoveChildNode
//  rather than as a function, so its 41 instructions are scored against this
//  one and it is written out inline here rather than called.  The helper
//  answers the predecessor the caller's walk would continue from, so this
//  function has to hand that value back too - declared `void` the whole
//  masked read is dead and seventeen instructions of the chunk disappear
//  with it.  Nothing reads the answer; the shipped code leaves whatever the
//  walk ended on in eax on the not-found path, which is what returning the
//  loop variable reproduces.
//============================================================================
#include "frame.h"
#include "framethunks.h"

static const char kSimpleFrameNodeTypeName[] = ".?AUSIMPLEFRAMENODE@@";

void CSimpleFrame::AddChildNode(void* child)
{
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
        nodeLink = (TSLink<SIMPLEFRAMENODE>*)&m_children.m_terminator;
    }

    nodeLink->Unlink();

    TSLink<SIMPLEFRAMENODE>* head = m_children.m_terminator.m_next;
    nodeLink->m_next = head;
    nodeLink->m_prevlink = head->m_prevlink;
    head->m_prevlink = (int)node;
    m_children.m_terminator.m_next = nodeLink;

    node->m_pFrame = child;
}

SIMPLEFRAMENODE* CSimpleFrame::RemoveChildNode(void* child)
{
    int link = (int)m_children.Tail();

    while (link > 0)
    {
        SIMPLEFRAMENODE* node = (SIMPLEFRAMENODE*)link;

        if (node->m_pFrame == child)
        {
            SIMPLEFRAMENODE* self = node;
            TSLink<SIMPLEFRAMENODE>* nodeLink =
                self != 0 ? &self->m_link
                          : (TSLink<SIMPLEFRAMENODE>*)&m_children.m_terminator;

            int prevLink = nodeLink->m_prevlink;
            SIMPLEFRAMENODE* prev =
                prevLink > 0 ? (SIMPLEFRAMENODE*)prevLink : 0;

            node->m_link.Unlink();
            g_simpleFrameNodeAllocator.Free(node, 0, 0);
            return prev;
        }

        link = (int)m_children.Prev(node);
    }

    return (SIMPLEFRAMENODE*)link;
}
