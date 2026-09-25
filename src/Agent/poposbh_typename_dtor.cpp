//============================================================================
//  NIpse::CPoPosBh - vtable slots 0/1/2 and the real destructor. See
//  poposbh.h for the class-level header comment.
//============================================================================
#include "poposbh.h"
#include "agentbasechain_staticpools.h"
#include "cellbuffer.h"
#include "storm.h"

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F48CAE0 - vtable slot 0. Literal "PoPosBh".
//----------------------------------------------------------------------------
const char* CPoPosBh::GetTypeName()
{
    return "PoPosBh";
}

//----------------------------------------------------------------------------
//  0x6F48CF00 - vtable slot 1.
//----------------------------------------------------------------------------
void CPoPosBh::ReturnToStaticPool()
{
    void* pool = CPoPosBh_GetStaticPool();

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

//----------------------------------------------------------------------------
//  0x6F48CFA0 - vtable slot 2, scalar deleting destructor. Calls the real
//  destructor first (which does its own restamp), then frees, guarded by
//  both the `flags & 1` bit and a `this != 0` check.
//----------------------------------------------------------------------------
void* CPoPosBh::ScalarDeletingDestructor(unsigned int flags)
{
    this->~CPoPosBh();

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
