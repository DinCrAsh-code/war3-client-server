//============================================================================
//  0x6F487DA0 - NTempest::CLinkedList<NIpse::CRlAgent>'s own vtable slot 2,
//  scalar deleting destructor - a separately-compiled, byte-identical
//  (per raw_bytes) twin of `NIpse::CRlAgent::ScalarDeletingDestructor`
//  (0x6F480410, crlagent.h/crlagent_scalardtor.cpp): CRlAgent's own slot 2
//  target is a DIFFERENT address (0x6F480410 vs this one, confirmed via
//  `worktree_store.py show 0x6F95207C --kind class`), so this is
//  `CLinkedList<CRlAgent>`'s own override, not an inherited copy of
//  CRlAgent's - the same duplicate-address shape
//  Agent/rlagentdef_scalardtor.cpp documents for `CLinkedList<CRlAgentDef>`.
//  Both call the same real `~CRlAgent()` (0x6F4803B0, crlagent_dtor.cpp)
//  and carry the `this != 0` guard.
//============================================================================
#include "crlagent.h"
#include "storm.h"

namespace NTempest {

class CLinkedList_CRlAgent
{
public:
    void* ScalarDeletingDestructor(unsigned int flags);
};

void* CLinkedList_CRlAgent::ScalarDeletingDestructor(unsigned int flags)
{
    ((NIpse::CRlAgent*)this)->~CRlAgent();

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NTempest
