//============================================================================
//  NIpse::CPmRegion - GetTypeName / ScalarDeletingDestructor. See pmregion.h;
//  ReturnToStaticPool and the two vtable-slot-3/4 forwarding stubs are their
//  own translation units (pmregion_returntopool.cpp/pmregion_slots34.cpp).
//============================================================================
#include "pmregion.h"
#include "storm.h"   // SMemFree (Storm_403)

namespace NIpse {

//  0x6F48AD40 - vtable slot 0.
const char* CPmRegion::GetTypeName()
{
    return "PmRegion";
}

//  0x6F48AE60 - vtable slot 2, scalar deleting destructor. See pmregion.h's
//  own declaration comment: no base teardown call at all, same open finding
//  as the rest of this base chain's own no-teardown destructors.
void* CPmRegion::ScalarDeletingDestructor(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@ - literal
                                          // address, not a symbol: see
                                          // CAgentBaseAbs::RestampToPresenceVtable's
                                          // own comment (cagentbaseabs_module_batch2.cpp)
                                          // for why (no real vtable-stamping
                                          // constructor exists yet for this class).

    if ((flags & 1) != 0 && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse
