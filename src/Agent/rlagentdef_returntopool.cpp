//============================================================================
//  0x6F488D90 - NIpse::CRlAgentDef::ReturnToStaticPool, vtable slot 1.  Same
//  unlink-then-return-to-pool shape as every sibling in this chain, against
//  CRlAgentDef's own static pool (CRlAgentDef_GetStaticPool,
//  Agent/agentbasechain_staticpools.h).
//============================================================================
#include "rlagentdef.h"
#include "agentbasechain_staticpools.h"

namespace NIpse {

void CRlAgentDef::ReturnToStaticPool()
{
    void* pool = CRlAgentDef_GetStaticPool();

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
