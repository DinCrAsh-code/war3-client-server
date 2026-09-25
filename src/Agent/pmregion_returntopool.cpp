//============================================================================
//  NIpse::CPmRegion::ReturnToStaticPool (0x6F48AE20, vtable slot 1). Own
//  translation unit, same reasoning as acceleratormap_returntopool.cpp's own
//  header comment (keeps this file's own pool-getter *declaration* clear of
//  pmregion.h's `#include "handletable.h"`, even though this particular body
//  does not itself need SHandleTableEntry - consistency with the rest of
//  this cluster's own file layout).
//============================================================================
#include "pmregion.h"
#include "agentbasechain_staticpools.h"   // PmRegion_GetStaticPool

namespace NIpse {

//  0x6F48AE20 - vtable slot 1.
void CPmRegion::ReturnToStaticPool()
{
    void* pool = PmRegion_GetStaticPool();

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
