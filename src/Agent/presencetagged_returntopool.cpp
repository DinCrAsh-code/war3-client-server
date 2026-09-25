//============================================================================
//  0x6F479CD0 - NTempest::CPresenceTagged::ReturnToStaticPool, vtable slot 1.
//  Identical shape to Agent/presence_returntopool.cpp's own
//  CPresence::ReturnToStaticPool, against CPresenceTagged's own pool
//  (CPresenceTagged_GetStaticPool, agentbasechain_staticpools.h).
//============================================================================
#include "presence.h"
#include "agentbasechain_staticpools.h"

namespace NTempest {

void CPresenceTagged::ReturnToStaticPool()
{
    void* pool = CPresenceTagged_GetStaticPool();

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
