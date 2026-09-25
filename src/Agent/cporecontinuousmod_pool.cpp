//============================================================================
//  0x6F48B090 - NIpse::CPoReContinuousMod::ReturnToStaticPool, vtable slot 1.
//============================================================================
#include "cporecontinuousmod.h"
#include "agentbasechain_staticpools.h"

namespace NIpse {

void CPoReContinuousMod::ReturnToStaticPool()
{
    void* pool = CPoReContinuousMod_GetStaticPool();

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
