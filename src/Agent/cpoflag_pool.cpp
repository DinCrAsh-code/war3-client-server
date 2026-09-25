//============================================================================
//  0x6F48D140 - NIpse::CPoFlag::ReturnToStaticPool, vtable slot 1. Same
//  unlink-then-push shape as every sibling in this family (Agent/cpolklis_pool.cpp),
//  against this class's own static pool (CPoFlag_GetStaticPool,
//  Agent/agentbasechain_staticpools.h/.cpp).
//============================================================================
#include "cpoflag.h"
#include "agentbasechain_staticpools.h"

namespace NIpse {

void CPoFlag::ReturnToStaticPool()
{
    void* pool = CPoFlag_GetStaticPool();

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

}  // namespace NIpse
