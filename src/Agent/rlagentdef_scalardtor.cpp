//============================================================================
//  0x6F488E40 - NIpse::CRlAgentDef::ScalarDeletingDestructor, vtable slot 2,
//  and 0x6F4890D0 - NTempest::CLinkedList_CRlAgentDef's own, byte-identical
//  apart from address (per raw_bytes) - two separately-compiled instances
//  of the same body, the same duplicate-address shape
//  Pathfinding/crlagent.h's own file header documents for 0x6F487DA0
//  (CLinkedList<CRlAgent>'s own copy of CRlAgent::ScalarDeletingDestructor).
//  Both call the real ~CRlAgentDef() first and carry the `this != 0` guard
//  on the free.
//============================================================================
#include "rlagentdef.h"
#include "storm.h"

namespace NIpse {

void* CRlAgentDef::ScalarDeletingDestructor(unsigned int flags)
{
    this->~CRlAgentDef();

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse

namespace NTempest {

void* CLinkedList_CRlAgentDef::ScalarDeletingDestructor(unsigned int flags)
{
    ((NIpse::CRlAgentDef*)this)->~CRlAgentDef();

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NTempest
