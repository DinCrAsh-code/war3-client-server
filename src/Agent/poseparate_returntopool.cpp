//============================================================================
//  0x6F484540 - NIpse::CPoSeparate::ReturnToStaticPool, vtable slot 1. Same
//  shape as Agent/presence_returntopool.cpp's own CPresence::ReturnToStaticPool:
//  unlink `this` from its own doubly-linked list of live instances
//  (+0x04/+0x08 prev/next), then push it onto CPoSeparate's own static
//  free-list pool (CPoSeparate_GetStaticPool,
//  agentbasechain_staticpools.h) - the intrusive link slot lives 4 bytes
//  before `this`.
//============================================================================
#include "poseparate.h"
#include "agentbasechain_staticpools.h"

namespace NIpse {

void CPoSeparate::ReturnToStaticPool()
{
    void* pool = CPoSeparate_GetStaticPool();

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
