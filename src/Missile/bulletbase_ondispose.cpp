//============================================================================
//  0x6F2BE7E0 - CBulletBase's vtable slot 46, the handler DispatchMessage
//  (bulletbase_dispatch.cpp) routes message id 0xD01A4 (852388) to, per
//  missile_thunderbolt.h's own note that this event id is the shared
//  observer-notify mechanism both the +0x2C and +0xA0 handle-ref slots
//  register for.
//
//  Best-effort, not EXACT/IDENTICAL: the shipped body carries a genuine
//  __except_handler4-shaped SEH frame (push -1 / push offset SEH_.. / mov
//  eax, fs:0 / the dword_6FAAE140 cookie XOR, matching fs:0 restore and
//  check on exit) this toolchain's fixed /GS- /EHs-c- cannot reproduce
//  (docs/msvc-vc8-idioms.md) - DIFFERS is the ceiling regardless of how
//  closely the body below matches the source it is a frame around.
//
//  Reading the body: construct a local resolved-reference from `msg`'s own
//  +0x0C field, resolve this object's own +0x2C target ref, and - if the
//  two name the same object - test that object's own +0x20 flag word
//  (bit 0x800000 clear) and, if so, set this object's own "target lost"
//  bit (0x10000000) in m_flags.  Either way, both locally-resolved
//  references are released (a manual refcount-decrement-then-vtable-slot-0
//  release, the same expansion CUnit::ClearMotionState's own +0x2C4
//  teardown already uses).
//============================================================================
#include "bulletbase.h"
#include "itemhandleresolve.h"

//  0x6F038450 - construct a refcounted-reference local from a resolved
//  object pointer, bumping its refcount.  Modelled as a plain "adopt and
//  addref" rather than reconstructed byte-for-byte; out of this pass's
//  budget (see docs/targets/CBulletBase.md).
static void* AdoptAndAddRef(void* obj)
{
    if (obj)
        ++*(int*)((char*)obj + 4);
    return obj;
}

typedef void (__thiscall *ReleaseSelfFn)(void*);

static void ReleaseLocalRef(void* obj)
{
    if (obj && --*(int*)((char*)obj + 4) == 0)
        ((ReleaseSelfFn)(*(void***)obj)[0])(obj);
}

void CBulletBase::OnTargetRefNotify(const SBulletDispatchMsg* msg)
{
    void* fromMsg = AdoptAndAddRef(*(void**)((const char*)msg + 0x0C));

    void* resolvedTarget = 0;
    if ((m_targetTypeTag & (int)m_targetHandle) != -1)
        resolvedTarget = QueryHandleField0x54((SOptionalHandleRef*)&m_targetHandle);
    void* fromSelf = AdoptAndAddRef(resolvedTarget);

    if (fromMsg == fromSelf && fromSelf != 0 &&
        !(*(unsigned int*)((char*)fromSelf + 0x20) & 0x800000))
    {
        m_flags |= 0x10000000u;
    }

    ReleaseLocalRef(fromSelf);
    ReleaseLocalRef(fromMsg);
}
