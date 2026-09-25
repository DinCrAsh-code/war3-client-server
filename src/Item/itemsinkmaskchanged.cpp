//============================================================================
//  0x6F39A3F0 - "the item's visibility mask changed": push the new mask
//  into the item's own widget-agent slot, then tell the renderer.
//
//  Was a `mov eax,<address> / jmp eax` redirect; fifteen instructions and
//  three calls, one of which (SWidgetAgentSlot::Assign, 0x6F085B50) is
//  already reconstructed here.
//
//  The slot is reached through a sub-object at +0x650 rather than directly:
//  0x6F2AEE80 is asked for it and hands back the address the assign runs
//  against.  Both that and the render-side notify at 0x6F38EA30 stay
//  thunks, with their shipped `retn <n>` recorded below.
//
//  Own translation unit: item_placegate.cpp's sink table reaches this with
//  a real call and it makes three of its own.
//============================================================================

//  0x6F085B50 - SWidgetAgentSlot::Assign, already reconstructed
//  (widgetagentslot.cpp).  Included, not re-declared: a local re-declaration
//  compiles and scores identically but mangles to a *different* symbol
//  (return type and parameter type are both part of the name), so the
//  reference goes unresolved at link time and nothing before build_mix.py
//  ever notices.  The header is the only spelling that cannot drift.
#include "widgetagentslot.h"

//----------------------------------------------------------------------------
//  0x6F2AEE80 - hand back the widget-agent slot the sub-object at +0x650
//  owns.  Not agent-registry bookkeeping - it is
//  TSGrowableArray<SWidgetAgentSlot>::New() (widgetagentslotarray.cpp),
//  grow-by-one-and-default-construct, confirmed against its own two growth
//  callees (0x6F2ABB10 ComputeChunk, 0x6F2ACCA0 SetAlloc) and against the
//  placement `new (slot) SWidgetAgentSlot(0)` its trailing call into
//  0x6F0955C0 makes.  `retn` with nothing on the stack matches New()'s own
//  no-stack-argument thiscall.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//  0x6F38EA30 - the renderer's own "this item's mask changed" notify: ~120
//  instructions of sprite and team-colour bookkeeping in the 0x6F38Exxx
//  rendering module.  `retn 4` - a thiscall member with one stack argument.
//  Genuinely attempted this session: its own body is straightforward, but it
//  calls six leaf helpers (0x6F2C6210/6220/6230/6240, 0x6F60FCE0, 0x6F2C64D0)
//  that have no dump anywhere in asm/ and whose `retn <n>` tools/ida_query.py
//  could not confirm this session (the operator's worker did not answer -
//  CLAUDE.md's "Exit 3 ... is never an answer", and this session's queries
//  to it timed out rather than coming back at all).  Every one of this
//  binary's calling conventions is callee-cleanup, so guessing an argument
//  count for even one of those six risks the exact live crash class
//  thunk_abi_audit.py exists to catch, with no way to check it from here.
//  Left a thunk rather than guess; ask IDA for those six bodies (or their
//  `func_at` + trailing-bytes retn) before attempting this one again.
//----------------------------------------------------------------------------
struct SItemMaskNotify
{
    void __thiscall OnMaskChanged(void* mask);
};

__declspec(naked) void __thiscall SItemMaskNotify::OnMaskChanged(void*)
{
    __asm
    {
        mov     eax, 06F38EA30h
        jmp     eax
    }
}

struct SItemSink2
{
    void OnMaskChanged(void* mask);

    char m_reserved00[0x650];
    //  +0x650 is the sub-object itself, not a pointer to one: the shipped
    //  code hands the slot query `lea ecx,[esi+650h]`, the address of the
    //  field.
    TSGrowableArray<SWidgetAgentSlot> m_slotOwner;    // +0x650
};

void SItemSink2::OnMaskChanged(void* mask)
{
    //  The mask is the slot's candidate agent; the shipped code hands the
    //  assign whatever the caller passed through untyped.
    m_slotOwner.New()->Assign((CAgent*)mask);
    ((SItemMaskNotify*)this)->OnMaskChanged(mask);
}
