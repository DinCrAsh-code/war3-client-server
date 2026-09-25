//============================================================================
//  0x6F48BE40 - NIpse::CPoFgLis::ReturnToStaticPool, vtable slot 1. Same
//  shape as Agent/cpolklis_pool.cpp's own CPoLkLis::ReturnToStaticPool -
//  g_pTimeSync + a fixed offset (CPoFgLis_GetStaticPool,
//  Agent/agentbasechain_staticpools.h/.cpp, kept out of this TU so /Ob2
//  cannot inline the two together).
//============================================================================
#include "cpofglis.h"
#include "agentbasechain_staticpools.h"

namespace NIpse {

void CPoFgLis::ReturnToStaticPool()
{
    void* pool = CPoFgLis_GetStaticPool();

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
