//============================================================================
//  0x6F605C70 - CLayoutFrame::RemoveDependency.  Its own translation unit:
//  ClearPoints, SetPoint, SetPointTo and SetAllPoints all *call* it in the
//  shipped build, so folding it in with any of them would inline it and lose
//  four real calls.
//
//  The walk runs backwards from the tail, which is the order the shipped code
//  has and not a detail: LinkToHead puts the newest node first, so the tail is
//  the oldest.  It enters on the masked Tail() and steps on the raw Prev(),
//  testing its sign - the pattern docs/msvc-vc8-idioms.md records for every
//  Storm list in this binary.
//============================================================================
#include "frame.h"
#include "framethunks.h"

//  0x6F605C70 - find this frame's node for `key` and drop it.  The walk runs
//  backwards from the tail, which is the order the shipped code has and not a
//  detail: LinkToHead puts the newest node first, so the tail is the oldest.
void CLayoutFrame::RemoveDependency(void* key)
{
    int link = (int)m_nodes.Tail();
    while (link > 0)
    {
        FRAMENODE* node = (FRAMENODE*)link;
        if (node->m_pKey == key)
        {
            node->Destroy(0);
            g_frameNodePool.Free(node, 0, 0);
            return;
        }
        link = (int)m_nodes.Prev(node);
    }
}

