//============================================================================
//  Vtable slots 0/1/2 for NIpse::CPrCluster/CPrClusterGen/CPrClusterGroup
//  and the matching slot-2 overrides of the two CLinkedList<T>
//  instantiations over them - see prcluster.h for the full family write-up.
//  Grouped in one file: none of these calls another body in this same
//  file (CPrCluster::ScalarDeletingDestructor calls the real ~CPrCluster(),
//  but only that destructor's DECLARATION is visible here -
//  prcluster_dtor.cpp's own header explains why its definition must not
//  be), so nothing here can inline into anything else here.
//============================================================================
#include "prcluster.h"
#include "storm.h"
#include "agentbasechain_staticpools.h"

namespace NIpse {

//----------------------------------------------------------------------------
//  NIpse::CPrCluster
//----------------------------------------------------------------------------

//  0x6F486B80
const char* CPrCluster::GetTypeName()
{
    return "PrCluster";
}

//  0x6F486BD0 - same shape as NTempest::CPresence::ReturnToStaticPool
//  (Agent/presence_returntopool.cpp).
void CPrCluster::ReturnToStaticPool()
{
    void* pool = CPrCluster_GetStaticPool();

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

//  0x6F487360 - calls the real ~CPrCluster() (prcluster_dtor.cpp, own TU so
//  it stays a real call here), guarded the same way
//  NTempest::CMemBlock::ScalarDeletingDestructor is.
void* CPrCluster::ScalarDeletingDestructor(unsigned int flags)
{
    this->~CPrCluster();

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

//----------------------------------------------------------------------------
//  NIpse::CPrClusterGen
//----------------------------------------------------------------------------

//  0x6F491780
const char* CPrClusterGen::GetTypeName()
{
    return "PrClusterGen";
}

//  0x6F491D20 - same unlink shape as CPrCluster/CPrClusterGroup's own above,
//  but the shipped body keeps `this` in ecx throughout instead of spilling
//  it into a `self` local first - written here without that local so the
//  compiler has no reason to spill either.
void CPrClusterGen::ReturnToStaticPool()
{
    void* pool = CPrClusterGen_GetStaticPool();

    void* prev = *(void**)((char*)this + 4);
    void* next = *(void**)((char*)this + 8);
    if (prev != 0)
        *(void**)((char*)prev + 8) = next;
    if (next != 0)
        *(void**)((char*)next + 4) = prev;
    *(void**)((char*)this + 4) = 0;
    *(void**)((char*)this + 8) = 0;

    SAgentChainPoolHeader* p = (SAgentChainPoolHeader*)pool;
    void* node = (char*)this - 4;
    *(void**)node = p->m_freeListHead;
    p->m_freeCount -= 1;
    p->m_freeListHead = node;
}

//  0x6F491D60 - the "trivial" no-teardown shape (prcluster.h's own header
//  comment): restamp straight to CPresence's own vtable, no base dtor
//  call, no `this != 0` guard on the free - same shape as
//  NTempest::CPresence::ScalarDeletingDestructorTrivial
//  (Agent/presence_dtor.cpp).
void* CPrClusterGen::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

//----------------------------------------------------------------------------
//  NIpse::CPrClusterGroup
//----------------------------------------------------------------------------

//  0x6F47AE70
const char* CPrClusterGroup::GetTypeName()
{
    return "PrClusterGroup";
}

//  0x6F47B0A0
void CPrClusterGroup::ReturnToStaticPool()
{
    void* pool = CPrClusterGroup_GetStaticPool();

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

//  0x6F47B0E0 - same "trivial" shape as CPrClusterGen's own above.
void* CPrClusterGroup::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse

namespace NTempest {

//----------------------------------------------------------------------------
//  NTempest::CLinkedList<T> slot-2 overrides - see prcluster.h's own header
//  comment on each struct for why these are separate, duplicate-compiled
//  bodies rather than real overrides.
//----------------------------------------------------------------------------

//  0x6F488620 - NTempest::CLinkedList<CPrCluster>'s own vtable slot 2.
void* CLinkedList_CPrCluster::ScalarDeletingDestructor(unsigned int flags)
{
    ((NIpse::CPrCluster*)this)->~CPrCluster();

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

//  0x6F486510 - NTempest::CLinkedList<CPrClusterGroup>'s own vtable slot 2.
void* CLinkedList_CPrClusterGroup::ScalarDeletingDestructorTrivial(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NTempest
