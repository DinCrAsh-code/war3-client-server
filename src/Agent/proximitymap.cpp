//============================================================================
//  NIpse::CProximityMap - GetTypeName / ReturnToStaticPool /
//  ScalarDeletingDestructor / the two own no-op vtable slots. See
//  proximitymap.h; InitGrid/Teardown (vtable slots 3/4) are their own
//  translation units (proximitymap_initgrid.cpp/proximitymap_teardown.cpp),
//  matching CLAUDE.md's "put a callee in its own TU" convention for the
//  larger bodies.
//============================================================================
#include "proximitymap.h"
#include "agentbasechain_staticpools.h"   // ProximityMap_GetStaticPool
#include "basemap.h"                      // SBaseMapRealDtorTarget (real dtor callee, out of scope)
#include "storm.h"                        // SMemFree (Storm_403)

namespace NIpse {

//  0x6F49FC30 - CProximityMap's own real destructor, out of this session's
//  own scope (see proximitymap.h's own header comment); tears its own
//  +0x8C/+0x6C sub-containers down before calling into
//  NIpse::CBaseMap_CPmCell's real destructor (0x6F49F190,
//  SBaseMapRealDtorTarget - basemap.h). File-scope, not local to
//  ScalarDeletingDestructor below: a local class whose member is declared
//  but not defined does not compile (`C3640`, docs/msvc-vc8-idioms.md's own
//  note on the identical restriction).
struct SProximityMapRealDtorTarget { void* RealDtor_6F49FC30(); };

//  0x6F49FB80 - vtable slot 0.
const char* CProximityMap::GetTypeName()
{
    return "ProximityMap";
}

//  0x6F49FBF0 - vtable slot 1.
void CProximityMap::ReturnToStaticPool()
{
    void* pool = ProximityMap_GetStaticPool();

    char* self = (char*)this;
    void* prev = *(void**)(self + 4);
    void* next = *(void**)(self + 8);
    if (prev != 0)
        *(void**)((char*)prev + 8) = next;
    if (next != 0)
        *(void**)((char*)next + 4) = prev;
    *(void**)(self + 4) = 0;
    *(void**)(self + 8) = 0;

    struct SPoolHeader { char m_reserved00[0x14]; void* m_freeListHead; unsigned int m_freeCount; };
    SPoolHeader* p = (SPoolHeader*)pool;
    void* node = self - 4;
    *(void**)node = p->m_freeListHead;
    p->m_freeCount -= 1;
    p->m_freeListHead = node;
}

//  0x6F49FCA0 - vtable slot 2, scalar deleting destructor.
void* CProximityMap::ScalarDeletingDestructor(unsigned int flags)
{
    ((SProximityMapRealDtorTarget*)this)->RealDtor_6F49FC30();

    if ((flags & 1) != 0 && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}

//  0x6F49FB90 / 0x6F49FBA0 - vtable slots 9/10 (+0x24/+0x28), no-ops.
struct SProximityMapNoOpArg
{
    void NoOp_6F49FB90(void*);
    void NoOp_6F49FBA0(void*);
};

void SProximityMapNoOpArg::NoOp_6F49FB90(void*) {}
void SProximityMapNoOpArg::NoOp_6F49FBA0(void*) {}

}  // namespace NIpse
