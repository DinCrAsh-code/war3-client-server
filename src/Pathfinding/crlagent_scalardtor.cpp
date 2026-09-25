//============================================================================
//  0x6F480410 - NIpse::CRlAgent::`scalar deleting destructor'(unsigned int),
//  vtable slot 2.  Calls the real, already-reconstructed ~CRlAgent()
//  (crlagent_dtor.cpp) first, then frees `this` when bit 0 of `flags` is
//  set, guarded by `this != 0` - the same shape
//  CAgentBaseAbs::ScalarDeletingDestructor
//  (Agent/cagentbaseabs_module_batch2.cpp) and
//  NTempest::CMemBlock::ScalarDeletingDestructor (Agent/cmemblock_dtor.cpp)
//  use whenever a real destructor call precedes the free - unlike
//  CPresence's/CPresenceTagged's/CPrRelation's own trivial versions
//  (Agent/presence.h's own header comment), which have neither.
//============================================================================
#include "crlagent.h"
#include "storm.h"

namespace NIpse {

void* CRlAgent::ScalarDeletingDestructor(unsigned int flags)
{
    this->~CRlAgent();

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
