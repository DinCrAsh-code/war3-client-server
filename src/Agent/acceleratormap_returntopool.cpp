//============================================================================
//  NIpse::CAcceleratorMap::ReturnToStaticPool (0x6F488960, vtable slot 1).
//  Own translation unit, same reasoning as acceleratormap.cpp's own header
//  comment: keeps this file (which only needs the pool getter's
//  declaration, not `game.h` itself) clear of basemap.h's own
//  `#include "handletable.h"`.
//============================================================================
#include "acceleratormap.h"
#include "agentbasechain_staticpools.h"   // AcceleratorMap_GetStaticPool

namespace NIpse {

//  0x6F488960 - vtable slot 1. Same unlink-then-return-to-pool shape as
//  every other ReturnToStaticPool in this base chain
//  (Agent/presence_returntopool.cpp), against this class's own real pool.
void CAcceleratorMap::ReturnToStaticPool()
{
    void* pool = AcceleratorMap_GetStaticPool();

    char* self = (char*)this;
    void* prev = *(void**)(self + 4);
    void* next = *(void**)(self + 8);
    if (prev != 0)
        *(void**)((char*)prev + 8) = next;
    if (next != 0)
        *(void**)((char*)next + 4) = prev;
    *(void**)(self + 4) = 0;
    *(void**)(self + 8) = 0;

    struct SPoolHeader { char m_reserved00[0x14]; void* m_freeListHead; unsigned int m_freeCount; };
    SPoolHeader* p = (SPoolHeader*)pool;
    void* node = self - 4;
    *(void**)node = p->m_freeListHead;
    p->m_freeCount -= 1;
    p->m_freeListHead = node;
}

}  // namespace NIpse
