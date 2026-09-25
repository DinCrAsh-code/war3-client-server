//============================================================================
//  0x6F3E4DB0 - CCameraSetup::~CCameraSetup().  Seventeen `mov`s, one per
//  property, each resetting that SUnitTrackedRef's own vtable pointer to
//  plain TRefCnt's (highest offset first - the shipped order), then the
//  base-class chain: ~CAgentWar3() re-stamps ??_7CAgentWar3@@6B@ and tail-
//  jumps into CAgent::~CAgent() (0x6F0017F0, agent_dtor.cpp) - both for
//  free from `: public CAgentWar3` once ~CAgentWar3() is small enough for
//  MSVC to inline into this destructor's own tail, which the shipped
//  instruction stream (the vtable stamp and the `jmp` sitting directly in
//  this function's own body, not a `call` out to it) says it was.
//============================================================================
#include "camerasetup.h"
#include "refcnt.h"

CCameraSetup::~CCameraSetup()
{
    TRefCnt stamp;
    void* vtbl = *(void**)&stamp;

    *(void**)&m_prop16 = vtbl;
    *(void**)&m_prop15 = vtbl;
    *(void**)&m_prop14 = vtbl;
    *(void**)&m_prop13 = vtbl;
    *(void**)&m_prop12 = vtbl;
    *(void**)&m_prop11 = vtbl;
    *(void**)&m_prop10 = vtbl;
    *(void**)&m_prop9  = vtbl;
    *(void**)&m_prop8  = vtbl;
    *(void**)&m_prop7  = vtbl;
    *(void**)&m_prop6  = vtbl;
    *(void**)&m_prop5  = vtbl;
    *(void**)&m_prop4  = vtbl;
    *(void**)&m_prop3  = vtbl;
    *(void**)&m_prop2  = vtbl;
    *(void**)&m_prop1  = vtbl;
    *(void**)&m_prop0  = vtbl;
}
