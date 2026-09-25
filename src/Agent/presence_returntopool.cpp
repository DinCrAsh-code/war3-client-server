//============================================================================
//  0x6F479B20 - NTempest::CPresence::ReturnToStaticPool, vtable slot 1.
//  Same shape as CAgentBaseAbs::ReturnToStaticPool
//  (Agent/cagentbaseabs_module_batch2.cpp): unlink `this` from its own
//  doubly-linked list of live instances (+0x04/+0x08 prev/next), then push
//  it onto CPresence's own static free-list pool
//  (CPresence_GetStaticPool, agentbasechain_staticpools.h) - the intrusive
//  link slot lives 4 bytes before `this`.
//============================================================================
#include "presence.h"
#include "agentbasechain_staticpools.h"

namespace NTempest {

void CPresence::ReturnToStaticPool()
{
    void* pool = CPresence_GetStaticPool();

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

}  // namespace NTempest
