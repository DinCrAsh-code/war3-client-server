//============================================================================
//  0x6F48B3B0 - NIpse::CPoRePulseMod::ReturnToStaticPool, vtable slot 1.
//============================================================================
#include "cporepulsemod.h"
#include "agentbasechain_staticpools.h"

namespace NIpse {

void CPoRePulseMod::ReturnToStaticPool()
{
    void* pool = CPoRePulseMod_GetStaticPool();

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
