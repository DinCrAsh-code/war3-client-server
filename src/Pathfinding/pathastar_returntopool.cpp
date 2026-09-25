//============================================================================
//  0x6F488BA0 - NIpse::CLrPathingAcc::ReturnToStaticPool, vtable slot 1.
//  Same unlink-then-return-to-pool shape as the rest of this cluster
//  (Pathfinding/pathclient_returntopool.cpp), against this class's own
//  static pool (CLrPathingAcc_GetStaticPool, Pathfinding/lrpath_staticpools.h).
//============================================================================
#include "pathastar.h"
#include "lrpath_staticpools.h"

void CPathAStar::ReturnToStaticPool()
{
    void* pool = CLrPathingAcc_GetStaticPool();

    char* self = (char*)this;
    void* prev = *(void**)(self + 4);
    void* next = *(void**)(self + 8);
    if (prev != 0)
        *(void**)((char*)prev + 8) = next;
    if (next != 0)
        *(void**)((char*)next + 4) = prev;
    *(void**)(self + 4) = 0;
    *(void**)(self + 8) = 0;

    SAgentChainPoolHeader* p = (SAgentChainPoolHeader*)pool;
    void* node = self - 4;
    *(void**)node = p->m_freeListHead;
    p->m_freeCount -= 1;
    p->m_freeListHead = node;
}
