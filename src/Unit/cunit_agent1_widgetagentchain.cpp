//============================================================================
//  0x6F28D5D0 - a depth-1/2 CUnit-vtable-BFS leaf (cunit_agent1_worklist.md).
//  `this`'s own +0x174/+0x178 SOptionalHandleRef pair is CUnit's own
//  m_ref174 (Unit/unit.h) - resolved through QueryHandleField0x54
//  (Item/itemhandleresolve.h) exactly as Unit/unit_chainhandlewalk.cpp's
//  own +0x1DC/+0x1E0 walk does it - and the result is chained into the
//  argument object's own +0x24 SOptionalHandleRef via
//  SOptionalHandleRefResolver::ResolveChained (Agent/handlereref_resolve.cpp,
//  already established), which is then chained right back the other way
//  into `this`'s own pair from the argument object itself.
//============================================================================
#include "itemhandleresolve.h"

//  Agent/handlereref_resolve.cpp - a real member of SOptionalHandleRef.
struct SOptionalHandleRefResolver : SOptionalHandleRef
{
    SOptionalHandleRef& ResolveChained(const void* ref);
};

struct SUnitWidgetAgentChainLeaf { void* __thiscall ChainWidgetAgentRef(void* other); };

//  0x6F28D5D0.  `retn 4`.
void* __thiscall SUnitWidgetAgentChainLeaf::ChainWidgetAgentRef(void* other)
{
    unsigned int* pLow  = (unsigned int*)((char*)this + 0x174);
    unsigned int* pHigh = (unsigned int*)((char*)this + 0x178);

    void* resolved = (*pHigh & *pLow) == 0xFFFFFFFF
        ? 0
        : QueryHandleField0x54((SOptionalHandleRef*)pLow);

    ((SOptionalHandleRefResolver*)((char*)other + 0x24))->ResolveChained(resolved);
    ((SOptionalHandleRefResolver*)pLow)->ResolveChained(other);

    return other;
}
