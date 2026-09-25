//============================================================================
//  0x6F480370 - NIpse::CRlAgent::ReturnToStaticPool, vtable slot 1.  Same
//  unlink-then-return-to-pool shape as this batch's own siblings one level
//  down the chain (Agent/presence_returntopool.cpp,
//  Agent/presencetagged_returntopool.cpp, Agent/agentrelation_returntopool.cpp),
//  against CRlAgent's own static pool (CRlAgent_GetStaticPool,
//  Agent/agentbasechain_staticpools.h).
//============================================================================
#include "crlagent.h"
#include "agentbasechain_staticpools.h"

namespace NIpse {

void CRlAgent::ReturnToStaticPool()
{
    void* pool = CRlAgent_GetStaticPool();

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
