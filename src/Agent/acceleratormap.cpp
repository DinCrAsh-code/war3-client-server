//============================================================================
//  NIpse::CAcceleratorMap - GetTypeName / ScalarDeletingDestructor / the two
//  own no-op vtable slots. See acceleratormap.h; ReturnToStaticPool (which
//  needs g_pTimeSync, i.e. Game/game.h) is its own translation unit
//  (acceleratormap_returntopool.cpp) to avoid the `SHandleTable` collision
//  between game.h and basemap.h's own `#include "handletable.h"`
//  (Agent/agentrelationteardown.cpp's own note on the identical conflict).
//============================================================================
#include "acceleratormap.h"
#include "basemap.h"   // SBaseMapRealDtorTarget
#include "storm.h"     // SMemFree (Storm_403)

namespace NIpse {

//  0x6F4888F0 - vtable slot 0.
const char* CAcceleratorMap::GetTypeName()
{
    return "AcceleratorMap";
}

//  0x6F4889B0 - vtable slot 2, scalar deleting destructor.
void* CAcceleratorMap::ScalarDeletingDestructor(unsigned int flags)
{
    ((SBaseMapRealDtorTarget*)this)->RealDtor_6F486A40();

    if ((flags & 1) != 0 && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

//  0x6F488900 / 0x6F488910 - vtable slots 9/10 (+0x24/+0x28), no-ops.
void SAcceleratorMapNoOpArg::NoOp_6F488900(void*) {}
void SAcceleratorMapNoOpArg::NoOp_6F488910(void*) {}

}  // namespace NIpse
