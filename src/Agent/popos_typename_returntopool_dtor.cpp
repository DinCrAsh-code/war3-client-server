//============================================================================
//  NIpse::CPoPos - vtable slots 0/1/2. See popos.h for the class-level
//  header comment.
//============================================================================
#include "popos.h"
#include "agentbasechain_staticpools.h"
#include "storm.h"

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F487680 - vtable slot 0. Literal "PoPos", no `this` read.
//----------------------------------------------------------------------------
const char* CPoPos::GetTypeName()
{
    return "PoPos";
}

//----------------------------------------------------------------------------
//  0x6F487A10 - vtable slot 1. Unlink `this` from its own +0x04/+0x08
//  live-instance list, then push it onto CPoPos's own static free-list
//  pool - same shape every sibling in this chain uses.
//----------------------------------------------------------------------------
void CPoPos::ReturnToStaticPool()
{
    void* pool = CPoPos_GetStaticPool();

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
//  0x6F487A50 - vtable slot 2, scalar deleting destructor. Byte-identical
//  to the whole chain's own no-base-teardown restamp-to-CPresence triplet
//  (Agent/presence.h's own header comment).
//----------------------------------------------------------------------------
void* CPoPos::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
