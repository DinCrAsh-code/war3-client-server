//============================================================================
//  0x6F48C330 - NIpse::CPoLkLis::ReturnToStaticPool, vtable slot 1. Same
//  shape as Agent/agentrelation_returntopool.cpp's own CAgentRelation::
//  ReturnToStaticPool - g_pTimeSync + a fixed offset (CPoLkLis_GetStaticPool,
//  Agent/agentbasechain_staticpools.h/.cpp - kept in that shared file, NOT
//  here alongside its caller, so /Ob2 cannot inline the two together; the
//  real dump shows the call staying out of line).  Unlink from the
//  doubly-linked list of live instances (own +4/+8 prev/next), push back
//  onto the free list at `this - 4`.
//============================================================================
#include "cpolklis.h"
#include "agentbasechain_staticpools.h"

namespace NIpse {

void CPoLkLis::ReturnToStaticPool()
{
    void* pool = CPoLkLis_GetStaticPool();

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
