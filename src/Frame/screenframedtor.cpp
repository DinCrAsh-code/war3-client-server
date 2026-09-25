//============================================================================
//  0x6F60A540 - CScreenFrame::~CScreenFrame.
//
//  Releases the two counted-reference members it declares itself
//  (m_pRenderObject, then m_168 - the shipped store order, not
//  declaration order) each with the same test-decrement-call shape
//  SAgentPtrSlot::Release (Misc/misc_agentslot_release.cpp, 0x6F388920)
//  already carries as its own standalone leaf, then falls through to the
//  compiler-generated teardown of m_refs[4] (an `eh vector destructor
//  iterator` over SFrameRef, which already has a real destructor -
//  framethunks.cpp, 0x6F60A430) and the base CFrame destructor
//  (0x6F608330, declared in frame.h, out of scope for this session - a
//  347-instruction-plus module of its own).
//
//  Behind the same `__except_handler4`-shaped SEH frame this toolchain
//  cannot reproduce (docs/msvc-vc8-idioms.md) that every destructible-
//  local-carrying function in this tree hits - the stack cookie chain, the
//  scope-table jump to __CxxFrameHandler3, and the two vtable stamps
//  (`??_7CScreenFrame@@6B@` at +0x00 for the CLayer base, and its `_0`
//  sibling at +0xB4 for the CLayoutFrame base - CFrame's own multiple
//  inheritance) a destructor always re-stamps on entry, ordinary C++
//  during unwinding (CLAUDE.md's ctor_vtable_audit note).
//
//  The IDA dump groups four more physical chunks under this same address -
//  0x6F60A350/0x6F60A3C0 (two more release-and-tail-jump leaves reached by
//  the two virtual-dtor adjustor thunks below) and 0x6F83F570/78/86/94
//  (out-of-line, far from this module: the scalar-deleting-destructor
//  adjustor thunks for both base vtables, plus the SEH filter/unwind
//  funclet) - none of them reconstructed here; they are MSVC's own thunk
//  furniture around a multi-base destructor, not this function's own body.
//
//  Own translation unit: the module's own destructor, called for real by
//  every deleting-destructor path CFrame-derived teardown reaches.
//============================================================================
#include "frame.h"
#include "agent.h"

//  0x6F608330 - CFrame::~CFrame.  Out of scope: a module of its own.
__declspec(naked) CFrame::~CFrame()
{
    __asm { mov eax, 06F608330h }
    __asm { jmp eax }
}

static void ReleaseAgentField(void* held)
{
    if (held)
    {
        CAgent* obj = (CAgent*)held;
        if (--obj->m_refcount == 0)
            obj->ReleaseSelf();
    }
}

CScreenFrame::~CScreenFrame()
{
    ReleaseAgentField(m_pRenderObject);
    ReleaseAgentField(m_168);
}
