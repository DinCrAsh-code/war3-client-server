//============================================================================
//  0x6F6066B0 - CLayoutFrame::AddDependency.  Its own translation unit
//  because SetPointTo and SetAllPoints both call it.
//
//  It links the node it just allocated *whether or not the allocation
//  succeeded*, and then writes through it.  That is not an oversight being
//  reproduced: it is what falls out of placement-new (whose own null test is
//  the `test eax,eax` ahead of the constructor) followed by an unconditional
//  LinkToHead, and it is why the shipped stream has a `lea ecx,[ebx+3Ch]` -
//  Link(0), the terminator - on the allocation-failed path.
//============================================================================
#include "frame.h"
#include "framethunks.h"

//  0x6F6066B0 - find this frame's node for `key`, or make one, and or
//  `flags` into it.
void CLayoutFrame::AddDependency(void* key, int flags)
{
    FRAMENODE* found = m_nodes.Tail();
    while ((int)found > 0)
    {
        if (found->m_pKey == key)
        {
            found->m_flags |= flags;
            return;
        }
        found = m_nodes.Prev(found);
    }

    FRAMENODE* node = (FRAMENODE*)g_frameNodePool.Alloc(
        1, ".?AUFRAMENODE@CLayoutFrame@@", -2);
    if (node)
        new (node) FRAMENODE;

    m_nodes.LinkToHead(node);
    node->m_pKey = key;
    node->m_flags = flags;
}
