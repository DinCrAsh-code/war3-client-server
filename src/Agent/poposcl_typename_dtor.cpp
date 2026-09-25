//============================================================================
//  NIpse::CPoPosCl / NTempest::CLinkedList<NIpse::CPoPosCl> - vtable slots
//  0/1/2. See poposcl.h for the class-level header comment.
//============================================================================
#include "poposcl.h"
#include "agentbasechain_staticpools.h"
#include "storm.h"

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F487B50 - vtable slot 0. Literal "PoPosCl". Shared with
//  CLinkedList<CPoPosCl>.
//----------------------------------------------------------------------------
const char* CPoPosCl::GetTypeName()
{
    return "PoPosCl";
}

//----------------------------------------------------------------------------
//  0x6F487BA0 - vtable slot 1. Shared with CLinkedList<CPoPosCl>, against
//  this class's own static pool (CPoPosCl_GetStaticPool).
//----------------------------------------------------------------------------
void CPoPosCl::ReturnToStaticPool()
{
    void* pool = CPoPosCl_GetStaticPool();

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
//  0x6F487BE0 - vtable slot 2, scalar deleting destructor. CPoPosCl's own
//  address; trivial no-base-teardown restamp triplet.
//----------------------------------------------------------------------------
void* CPoPosCl::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

//----------------------------------------------------------------------------
//  0x6F488670 - CLinkedList<CPoPosCl>'s own override of vtable slot 2.
//  Byte-identical to CPoPosCl::ScalarDeletingDestructorTrivial above -
//  own address (this image links without /OPT:ICF), same triplet.
//----------------------------------------------------------------------------
void* CLinkedList_CPoPosCl::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
