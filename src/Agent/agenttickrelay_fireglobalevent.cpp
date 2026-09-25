//============================================================================
//  0x6F4AA6E0 - CAgentTickRelay::FireGlobalAgentEvent.  Own translation
//  unit, deliberately separate from agenttickrelay_tick.cpp: that file
//  holds this function's only caller (Reload), and with both the caller
//  and this function's whole body visible in one TU, `/Ob2` inlines this
//  straight into Reload - losing the real `call CAgentTickRelay::
//  FireGlobalAgentEvent` the shipped body's own cold tail chunk makes
//  (confirmed by comparing generated output with/without the split: with
//  both in one file, Reload's own score dropped 29/38 -> 27/38, the
//  inlined body replacing what should be a five-instruction call site).
//  Same "own TU so a real call stays real" reasoning as prcluster_dtor.cpp/
//  Agent/agentbasechain_staticpools.cpp elsewhere in this codebase.
//============================================================================
#include "agenttickrelay.h"
#include "game.h"

//  0x6F483290 - shared "select subsystem" leaf (Agent/prbehaviorloadhelpers.cpp/
//  prbehavior_load.cpp's own copy of the same declaration).  `ecx` is the
//  fixed CTimeSync* global (g_pTimeSync), the one stack dword is `this`.
struct STimeSyncPoolSelector
{
    void* SelectSubsystem(void* self);
};

//----------------------------------------------------------------------------
//  0x6F4AA630 - `sub_6F4AA6E0`'s own registration callee.  `this` (ecx) is
//  the subsystem pointer `STimeSyncPoolSelector::SelectSubsystem` returns;
//  its own body (increments the subsystem's own +0x50 sequence counter,
//  clamps a float read through `slot` against a floor constant, then
//  builds an argument list and calls `sub_6F4AA5A0`) bottoms out in that
//  same "genuinely new allocator pattern" leaf `Agent/prbehaviorloadhelpers.cpp`
//  already documents as out of scope for this cluster (it is the same
//  `sub_6F4AA5A0` that leaf's own 0x6F4AA720 reaches) - kept a naked
//  redirect rather than growing this session's scope to reconstructing a
//  brand-new allocator.  `__thiscall`, three stack dwords (relay `this`,
//  address of the caller's own `fourCC` local, `slot`), `retn 0Ch`.
//----------------------------------------------------------------------------
struct STimeSyncSubsystem
{
    void* RegisterPendingEvent(void* relayThis, const unsigned int* fourCC, void* slot);
};

__declspec(naked) void* STimeSyncSubsystem::RegisterPendingEvent(void*, const unsigned int*, void*)
{
    __asm { mov eax, 06F4AA630h
            jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F4AA6E0 - vtable-adjacent helper (not itself a vtable slot) that
//  fires a second, FourCC-keyed observer event through a caller-supplied
//  `slot` pointer and arms `m_pendingB` with whatever the registration call
//  hands back.  Same "flag the old target's own +0x10 word, then replace
//  the reference" shape ReleasePendingA/B's own doc comment gives for this
//  field pair.
//----------------------------------------------------------------------------
void __thiscall CAgentTickRelay::FireGlobalAgentEvent(void* slot, unsigned int fourCC)
{
    if (m_pendingB)
        *(unsigned int*)((char*)m_pendingB + 0x10) |= 0x10000;

    m_pendingB = ((STimeSyncSubsystem*)((STimeSyncPoolSelector*)g_pTimeSync)->SelectSubsystem(this))
                     ->RegisterPendingEvent(this, &fourCC, slot);
}
