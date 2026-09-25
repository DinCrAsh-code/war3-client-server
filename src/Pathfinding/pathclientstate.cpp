//============================================================================
//  The small pieces of CPathClient's own state machine: the two one-line
//  setters the order side pokes at, the search-state reset, the cursor
//  rewind and the best-node record.
//
//  They share a translation unit because they share nothing else - each is
//  reached from a different level of the tree and none of them calls any of
//  the others, so nothing here can be inlined away by accident.
//============================================================================
#include "pathmove.h"

//----------------------------------------------------------------------------
//  0x6F49AD10 - reached off SHandleWithType::PropagatePlacementIndex
//  (Widget/widget_handlebits.cpp), which resolves this client through the
//  global handle table before calling here.  The mask arrives as a
//  pointer and is dereferenced once, the same shape
//  CPathSearch::TestFootprint (pathfootprintsize.cpp) already uses for
//  its own +0xA4 twin.
//----------------------------------------------------------------------------
void CPathClient::SetTypeMask(const int* mask)
{
    m_typeMask = *mask;
}

//----------------------------------------------------------------------------
//  0x6F49AD30 - point the client at a region node.  The caller owns the
//  reference; this only records it.
//----------------------------------------------------------------------------
void CPathClient::SetTargetNode(CGridRegistration* node)
{
    m_targetNode = node;
}

//----------------------------------------------------------------------------
//  0x6F49AD40 - copy in the footprint radius the search sizes cells by.
//  Taken by address, not by value: the caller keeps it in a local.
//----------------------------------------------------------------------------
void CPathClient::SetRadius(const CFloat* radius)
{
    m_radius = *radius;
}

//----------------------------------------------------------------------------
//  0x6F49B3E0 - forget everything about the search in flight: the route is
//  no longer valid, no retries are outstanding, and the next tick may repath
//  immediately.
//----------------------------------------------------------------------------
void CPathClient::ClearSearchState()
{
    m_flags &= ~kPathFlagValid;
    m_retries = 0;
    m_cooldown = 0;
}

//----------------------------------------------------------------------------
//  0x6F49B400 - put one cache's cursor back on its last point.
//
//  The `slot != 0` guard is the shipped code's, not a tidy-up: only slot 0
//  steps an extra point back, and only when there is one to step back to.
//  Slot 1's cursor is allowed to land on the count itself.
//----------------------------------------------------------------------------
void CPathClient::RewindCursor(int slot)
{
    //  The caches are 0x20 bytes apart and only the first is a declared
    //  member, so the pair is indexed off its address - see pathmove.h.
    unsigned int last = m_cache[slot].m_count - 1;
    if (slot == 0 && last > 0)
        --last;
    m_cursor[slot] = last;
}

//----------------------------------------------------------------------------
//  0x6F49B430 - record the node the flood settled on, and make sure the
//  repath cooldown is at least `cooldown` ticks.
//
//  A null node stores the -1 sentinel in *both* halves of the handle, which
//  is what the reader at 0x6F49DF00 tests: it looks the handle up and treats
//  a failed lookup as "no node".
//----------------------------------------------------------------------------
void CPathClient::SetBestNode(const CGridRegistration* node,
                              unsigned int cooldown)
{
    int tag;
    if (node != 0)
    {
        m_bestHandle = ((const unsigned int*)node)[5];   // node->+0x14
        tag = ((const int*)node)[6];                     // node->+0x18
    }
    else
    {
        tag = -1;
        m_bestHandle = (unsigned int)tag;
    }
    m_bestTypeTag = tag;

    if (m_cooldown < cooldown)
        m_cooldown = cooldown;
}
