//============================================================================
//  0x6F480060 - NIpse::CPrRelation::ReturnToStaticPool (`CAgentRelation::
//  ReturnToStaticPool`), vtable slot 1.  Same unlink-then-return-to-pool
//  shape as Agent/presence_returntopool.cpp's own
//  CPresence::ReturnToStaticPool/CPresenceTagged::ReturnToStaticPool,
//  against this class's own static pool
//  (CPrRelation_GetStaticPool, Agent/agentbasechain_staticpools.h).
//============================================================================
#include "agentrelation.h"
#include "agentbasechain_staticpools.h"

void CAgentRelation::ReturnToStaticPool()
{
    void* pool = CPrRelation_GetStaticPool();

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
