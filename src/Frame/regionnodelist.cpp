//============================================================================
//  0x6F609730 - CRegionNodeList::RemoveNode: unlink one REGIONNODE, give it
//  back to the pool, and answer the predecessor the caller's walk should
//  continue from.
//
//  The masked predecessor is read *before* the unlink, which is what makes
//  it the return value rather than a re-read afterwards.
//============================================================================
#include "frame.h"
#include "framethunks.h"

REGIONNODE* CRegionNodeList::RemoveNode(REGIONNODE* node)
{
    REGIONNODE* prev = Prev(node);
    int link = (int)prev;
    REGIONNODE* answer = link > 0 ? (REGIONNODE*)link : 0;

    node->m_link.Unlink();
    g_regionNodeAllocator.Free(node, 0, 0);

    return answer;
}
