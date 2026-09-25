//============================================================================
//  0x6F3EAD80 - CCameraSetup::DeleteSelf(int flags), vtable slot 1.  The
//  same "destruct, then free through Storm if bit 0 is set" shape this
//  repo already has on CAgent/TRefCnt: call the destructor, and only if
//  (flags & 1) *and* `this` is non-null, free through Storm's own
//  allocator tagged "delete".
//============================================================================
#include "camerasetup.h"
#include "storm.h"

CAgentWar3* CCameraSetup::DeleteSelf(int flags)
{
    this->~CCameraSetup();
    if ((flags & 1) && this)
        SMemFree(this, "delete", -1, 0);
    return this;
}
