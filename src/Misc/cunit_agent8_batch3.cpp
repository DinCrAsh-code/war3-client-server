//============================================================================
//  createunit-G4 cluster C - three more leaves from CUnit's own vtable BFS
//  closure, no owning class otherwise identified (same family as
//  cunit_agent8_batch2.cpp's SDualHandleRefHost6F2E37E0, a different
//  parallel agent's own slice - independently offset here per this repo's
//  usual "redeclare rather than share" convention, CLAUDE.md/
//  flagbit2cc_setbitbylookup.cpp).
//============================================================================
#include "missile_thunderbolt.h"
#include "unitorder.h"

//  0x6F2AB3E0 - the real sibling registration entry point, reconstructed by
//  a different parallel agent as a standalone leaf (misc_notifyorforward_
//  event.cpp, EXACT) rather than as a CMissileThunderBoltRefs member; called
//  through this locally-redeclared dispatch struct, same as that file's own
//  cunit_agent8_batch2.cpp caller.
struct SEventNotifyOrForwardHost
{
    void NotifyOrForwardEvent(void* target, int flag);
};

//----------------------------------------------------------------------------
//  0x6F2E3790 - update `this`'s own +0x48 handle-ref slot to `newTarget`:
//  if the slot currently resolves to something, unregister `this` as its
//  observer (through NotifyOrForwardEvent); then unconditionally
//  resolve+store `newTarget` into the +0x48 slot
//  (SOptionalHandleRefResolver::ResolveChained), register `this` as
//  `newTarget`'s observer, and call Finalize() - note the resolve happens
//  *before* the register, the opposite order from what a "release old,
//  then acquire new" reading would suggest.  Unlike cunit_agent8_batch2.cpp's
//  UpdateRef54 (the same
//  overall shape at +0x54/+0x60), this one has no "already resolved to
//  something other than newTarget" branch - it always re-registers and
//  re-resolves.  `this`'s real class is not established; only the one
//  offset this function itself touches is named.
//----------------------------------------------------------------------------
struct SHandleRefHost6F2E3790
{
    void UpdateRef48(void* newTarget);
    void Finalize();   // 0x6F2DEEF0 - not this agent's address, redirect-thunked below

    char                          m_reserved00[0x48];
    SOptionalHandleRefResolver    m_ref48;   // +0x48
};

void SHandleRefHost6F2E3790::UpdateRef48(void* newTarget)
{
    void* resolved = 0;
    if ((m_ref48.m_typeTag & (int)m_ref48.m_handle) != -1)
        resolved = QueryHandleField0x54(&m_ref48);

    if (resolved)
        ((SEventNotifyOrForwardHost*)resolved)->NotifyOrForwardEvent(this, 0);

    m_ref48.ResolveChained(newTarget);
    ((SEventNotifyOrForwardHost*)newTarget)->NotifyOrForwardEvent(this, 1);

    Finalize();
}

__declspec(naked) void SHandleRefHost6F2E3790::Finalize()
{
    __asm { mov eax, 06F2DEEF0h }
    __asm { jmp eax }
}
