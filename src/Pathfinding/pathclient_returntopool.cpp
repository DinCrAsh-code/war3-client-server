//============================================================================
//  0x6F49CD70 - NIpse::CLrPath::ReturnToStaticPool, vtable slot 1.  Same
//  unlink-then-return-to-pool shape as Agent/presence_returntopool.cpp's
//  whole family (CPresence/CPresenceTagged/CPrRelation/CRlAgent), against
//  this class's own static pool (CLrPath_GetStaticPool,
//  Pathfinding/lrpath_staticpools.h).
//============================================================================
#include "pathmove.h"
#include "lrpath_staticpools.h"

void CPathClient::ReturnToStaticPool()
{
    void* pool = CLrPath_GetStaticPool();

    void* prev = *(void**)((char*)this + 4);
    void* next = *(void**)((char*)this + 8);
    if (prev != 0)
        *(void**)((char*)prev + 8) = next;
    if (next != 0)
        *(void**)((char*)next + 4) = prev;
    *(void**)((char*)this + 4) = 0;
    *(void**)((char*)this + 8) = 0;

    SAgentChainPoolHeader* p = (SAgentChainPoolHeader*)pool;
    void* node = (char*)this - 4;
    *(void**)node = p->m_freeListHead;
    p->m_freeCount -= 1;
    p->m_freeListHead = node;
}
